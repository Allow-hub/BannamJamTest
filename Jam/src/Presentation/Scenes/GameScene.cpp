#include "Presentation/Scenes/GameScene.h"
#include "Presentation/Scenes/Transition/TransitionManager.h"

namespace Jam::Presentation::Scenes
{
	GameScene::GameScene(const InitData& init)
		: IScene{ init },
		m_world({ 0, 980 }),//引数は重力
		m_inputManager()
	{
		Jam::Presentation::AudioService::get().play(Jam::Presentation::AudioService::Sound::BGM_Game, true);

		auto& core = Jam::Foundation::CoreManager::Instance();
		
		if (!core.isEditorMode()) core.reset();
		
		String stageName;
		if (core.isEditorMode()) stageName = U"stage_edited.json";
		else stageName = Jam::Foundation::CoreManager::stageNameToString(core.stageInfo.stageName);
		
		core.setCurrentStageData(core.getStageData(core.stageInfo.stageName));

		// --- FactoryServiceLocator初期化 ---
		auto& locator = Jam::Infrastructure::Locator::FactoryServiceLocator::instance();
		auto physicsFactory = std::make_shared<Jam::Infrastructure::Locator::Siv3DPhysicsBodyFactory>();
		physicsFactory->initialize(m_world);
		// ボディ作成時に自動的にm_physicsBodiesに追加するコールバックを設定
		physicsFactory->setBodyCreatedCallback([this](auto body) {
			m_physicsBodies.push_back(body);
		});
		locator.registerPhysicsFactory(physicsFactory);

		// EnemyFactory初期化と登録
		m_enemyFactory = std::make_unique<Jam::UseCase::EnemyFactory>();
		locator.registerEnemyFactory(m_enemyFactory.get());


		// === Game内のイベント用クラスを初期化 ===
		m_gameEventQueue = std::make_shared<Jam::Domain::Events::GameEventQueue>();
		m_cameraEventQueue = std::make_shared<Jam::UseCase::CameraEventQueue>();
		m_effectEventQueue = std::make_shared<Jam::UseCase::EffectEventQueue>();

		m_effectManager = std::make_unique<Jam::Presentation::EffectManager>(*m_effectEventQueue);

		// === Player 初期化 ===
		auto stats = Jam::Infrastructure::Physics::LoadFromJSON(U"Assets/Player/player_stats.json");

		auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
			.getPhysicsFactory()
			->createBody(
			Vec2{ 0, -5 },//地面に埋まらないように
			SizeF{ 50, 120 },
			s3d::P2BodyType::Dynamic,
			stats.physicsMaterial
			);

		// === Player 初期化 ===
		m_player = std::make_shared<Domain::Player::Player>(playerBody, *m_gameEventQueue);
		playerBody->setLayer(Jam::Domain::Physics::PhysicsLayer::Player);
		playerBody->setCollisionListener(m_player);

		m_player->setPower(stats.power);
		m_player->setHp(stats.hp);
		m_player->setMaxHp(stats.hp);
		m_player->setSpeed(stats.moveSpeed);
		m_player->setJumpPower(stats.jumpPower);

		m_playerManager = std::make_unique<Jam::Presentation::PlayerManager>(m_player);
		m_playerService = std::make_unique<Jam::UseCase::PlayerService>(
			m_player,
			m_inputManager,
			*m_playerManager
		);

		// === Stage 初期化 ===
		// ステージテクスチャの事前読み込み
		Jam::Infrastructure::TextureLoader::preloadStageTextures();

		m_stageService = std::make_shared<Jam::UseCase::StageService>();

		auto bodyFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
			.getPhysicsFactory();

		// ステージ名からJSONファイル名を生成
		// イベントキューとプレイヤーIDを渡す（ダメージ床用）
		// stageName に .json が含まれていない場合のみ追加
		String stageFilePath = stageName.ends_with(U".json") ? stageName : (stageName + U".json");
		m_stageService->initialize(stageFilePath, bodyFactory, *m_gameEventQueue, playerBody->getID());

		// ステージの物理ボディをGameSceneの物理ボディリストに追加（衝突検出用）
		for (const auto& stageBody : m_stageService->getPhysicsBodies()) {
			auto siv3dBody = std::dynamic_pointer_cast<Infrastructure::Physics::Siv3DPhysicsBody>(stageBody);
			if (siv3dBody) {
				m_physicsBodies.push_back(siv3dBody);
			}
		}

		m_stageManager = std::make_unique<Jam::Presentation::Stage::StageManager>();
		m_stageManager->setService(m_stageService);
		m_stageManager->loadTextures();

		// === Camera 初期化 ===
		m_cameraManager = std::make_shared<Jam::Presentation::CameraManager>(
				m_player->getPosition()  // 初期位置をプレイヤー位置に合わせる
		);
		m_cameraManager->setYLimits(-1000, core.getCurrentStageData().fallLimitY - 500);
		m_cameraService = std::make_shared<Jam::UseCase::CameraService>(
			*m_player,
			*m_cameraManager,
			*m_cameraEventQueue
		);
		Jam::Infrastructure::CursorUtil::instance().setCameraManager(m_cameraManager);
		// === Enemy 初期化 ===
		m_enemyManager = std::make_unique<Jam::Presentation::EnemyManager>();


		// 敵ステータスをJSONからロード
		std::unordered_map<Jam::Domain::EnemyType, Jam::Domain::Enemy::EnemyStatus> enemyStatusTable;
		if (Jam::Infrastructure::EnemyLoader::loadEnemyStatusFromJSON(
			U"Assets/Enemy/enemy_stats.json",
			enemyStatusTable))
		{
			m_enemyFactory->setStatusTable(enemyStatusTable);
		}
		else
		{
			Console << U"[GameScene] ⚠ Failed to load enemy status";
		}

		// プレイヤーの Body ID を取得
		auto playerBodyId = playerBody->getID();

		// GameEventHandlerを初期化
		m_eventHandler = std::make_shared<Jam::UseCase::GameEventHandler>(
			*m_gameEventQueue,
			*m_cameraEventQueue,
			*m_effectEventQueue,
			[this]() { this->nextScene(); },
			m_enemyManager.get()
		);

		// ステージ用敵配置 JSON をロードして敵を生成
		String enemyFilePath;
		if (core.isEditorMode())
			enemyFilePath = U"Assets/Enemy/enemy_edited.json";
		else
			enemyFilePath = U"Assets/Enemy/enemy_" + stageName + U".json";
		
		if (!Jam::Infrastructure::EnemyLoader::loadEnemyForStageFromJSON(
			enemyFilePath,
			m_enemyFactory,
			m_enemyManager,
			playerBodyId, *m_gameEventQueue))
		{
			Console << U"[GameScene] ⚠ Failed to load stage enemies";
		}

		// === Background 初期化 ===
		m_backgroundRenderer = std::make_unique<Jam::Presentation::Background::BackgroundRenderer>();

		m_inGameUIManager = std::make_unique<Jam::Presentation::InGameUIManager>(m_player);
		// 背景テクスチャの事前読み込み
		Jam::Infrastructure::TextureLoader::preloadBackgroundTextures();

		// JSONから背景データを読み込み（ステージ名を渡す）
		Array<Jam::Domain::Background::BackgroundObject> backgroundObjects;
		if (Jam::Infrastructure::Background::BackgroundLoader::loadBackgroundFromFile(U"background.json", core.stageInfo.stageName, backgroundObjects)) {
			m_backgroundRenderer->setBackgroundObjects(backgroundObjects);
		}
		else {
			Console << U"[GameScene] ⚠️ Failed to load background JSON, using fallback";
		}

		auto currentStageData = core.getCurrentStageData();
		for (const auto& pos : currentStageData.flagmentMemoryPos)
		{
			auto flagmentBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()
				->createCircleSensor(
					pos,
					30.0,  // センサーの半径
					s3d::P2BodyType::Static
				);

			flagmentBody->setLayer(Jam::Domain::Physics::PhysicsLayer::Item);

			// FlagmentMemoryインスタンスを作成
			auto flagment = std::make_shared<Jam::Domain::FlagmentMemory>(pos, flagmentBody);

			// 物理ボディにリスナーを設定
			flagmentBody->setCollisionListener(flagment);

			// 管理配列に追加
			m_flagmentMemories.push_back(flagment);
		}

		if (core.stageInfo.stageName == Jam::Foundation::StageName::Stage1_3)
		{
			m_cameraManager->lockFocusOn({ 250,-500 }, 0.7);
		}

		// === Goal 初期化 ===
		Vec2 goalPosition = core.getCurrentStageData().goalData.position;
		Vec2 goalSize = core.getCurrentStageData().goalData.size;
		
		// エディタモードの場合、JSONからゴール位置を読み込み
		if (core.isEditorMode())
		{
			const JSON stageJson = JSON::Load(U"Assets/Stage/stage_edited.json");
			if (stageJson && stageJson.hasElement(U"goal"))
			{
				const auto& goalJson = stageJson[U"goal"];
				if (goalJson.hasElement(U"position") && goalJson[U"position"].isArray())
				{
					const auto& posArray = goalJson[U"position"];
					if (posArray.size() >= 2)
					{
						goalPosition = Vec2(posArray[0].get<double>(), posArray[1].get<double>());
					}
				}
				if (goalJson.hasElement(U"size") && goalJson[U"size"].isArray())
				{
					const auto& sizeArray = goalJson[U"size"];
					if (sizeArray.size() >= 2)
					{
						goalSize = Vec2(sizeArray[0].get<double>(), sizeArray[1].get<double>());
					}
				}
			}
		}
		
		auto goalBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
			.getPhysicsFactory()->createRectSensor(goalPosition, goalSize);
		auto goal = std::make_shared<Jam::Domain::GoalArea>(
			goalBody,
			[this]() { this->pushDeferredAction([this]() { this->nextScene(); }); },	// nextSceneをDeferredActionとして登録
			*m_effectEventQueue,
			*m_cameraEventQueue
		);
		// プレイヤーへ即時通知
		goal->setOnPlayerClear([this, goalBody]() {
			if (m_playerService)
			{
				// ゴールの物理ボディ座標
				m_playerService->onGoalReached(goalBody->getPosition());
			}
		});

		goalBody->setCollisionListener(goal);
		Jam::Infrastructure::IndependentObjectFactory::instance().registerObject(goal);

		Jam::Util::GridRenderer::GridConfig config;
		config.gridSize = 100.0;
		config.fontSize = 16;
		Jam::Util::GridRenderer::instance().setConfig(config);
		// 元画像を読み込む
		Image originalImage(Resource(U"Assets/Cursor/cursor_yellow.png"));
		originalImage = originalImage.scaled(64, 64);
		// カーソル登録
		Jam::Infrastructure::CursorUtil::instance().registerCustomCursor(
			U"Cross",
			originalImage,
			Point{ originalImage.width() / 2, originalImage.height() / 2 } // hotSpotは中央
		);
		BloomManager::getInstance().setIntensities(0.26, 0.12, 0.22, 0.15);
		BloomManager::getInstance().setVignette(0.3, 0.5);
	}

	void GameScene::update() 
	{
		auto& core = Jam::Foundation::CoreManager::Instance();
		core.addTimer(Scene::DeltaTime());

		auto& cursorUtil = Jam::Infrastructure::CursorUtil::instance();
		cursorUtil.requestStyle(U"Cross");
		cursorUtil.setClipWindowCuror(true);
		m_playerService->update(Scene::DeltaTime());
		m_playerManager->update();
		if (core.getPause())
		{
			Jam::Presentation::SettingManager::Instance().update();
			return;
		}
		// Stage更新
		if (m_stageManager)
		{
			m_stageService->update(Scene::DeltaTime());
		}

		// 敵の更新
		if (m_enemyManager)
		{
			m_enemyManager->update(Scene::DeltaTime());
		}

		//単体オブジェクトの更新
		Jam::UseCase::IndependentObjectService::update(Scene::DeltaTime());


		constexpr double StepTime = 1.0 / 400.0;
		m_accumulatedTime += Scene::DeltaTime();

		while (StepTime <= m_accumulatedTime)
		{
			const auto collisions = m_world.getCollisions();
			notifyCollisionEvents(collisions);

			m_world.update(StepTime);
			m_accumulatedTime -= StepTime;
		}
		m_eventHandler->processEvents();

		if (m_stageService && m_player)
		{
			bool landed = m_stageService->checkOneWayPlatformLanding(
				m_player->getPhysicsBody(),
				m_player->isPressingDown()
			);
			if (landed)
			{
				m_player->resetJumpState();
			}

		}

		m_cameraService->update(Scene::DeltaTime());
		m_effectManager->update();
		Jam::Presentation::FadeManager::instance().update(Scene::DeltaTime());

		//記憶のかけら
		for (const auto& flagment : m_flagmentMemories)
		{
			flagment->update(Scene::DeltaTime());
		}
		m_inGameUIManager->update();

		//デバッグ用
		if (KeyR.down())
		{
			resetScene();
			changeScene(ToSceneString(SceneName::InGame));
		}

		//DeferredActionの実行
		for (auto& action : m_deferredActions) {
			if (action) action();
		}
		m_deferredActions.clear();
	}

	void GameScene::draw() const
	{
		Scene::SetBackground(ColorF{ 0.9, 0.9, 1.0 });

		{
			if (Jam::Foundation::CoreManager::Instance().getPause())
			{
				Jam::Presentation::SettingManager::Instance().draw();
				return;
			}
			const auto t = Jam::Presentation::BloomManager::getInstance().getRenderTarget();
			const auto transformer = m_cameraManager->createTransformer();
			const Vec2 cameraOffset = m_cameraManager->getCameraOffset();

			// === 背景描画 (奥から手前へ) ===
			if (m_backgroundRenderer) {
				if (m_backgroundRenderer->isLoaded()) {
					m_backgroundRenderer->drawLayer(Jam::Domain::Background::ParallaxLayer::Back, cameraOffset);
					m_backgroundRenderer->drawLayer(Jam::Domain::Background::ParallaxLayer::Middle, cameraOffset);
				}
			}

			// Stage描画
			if (m_stageManager)
			{
				m_stageManager->draw();
			}

			// === デバッグ: PhysicsLayer可視化(常時表示) ===
			if (m_stageService)
			{
				m_stageService->drawPhysicsLayerDebug();
			}

			//単体オブジェクトの描画
			Jam::Presentation::IndependentObjectManager::draw();

			// プレイヤーの描画
			m_playerManager->draw();				// === 前景背景描画 (プレイヤーより手前) ===
			if (m_backgroundRenderer && m_backgroundRenderer->isLoaded()) {
				m_backgroundRenderer->drawLayer(Jam::Domain::Background::ParallaxLayer::Front, cameraOffset);
			}

			// 敵の描画
			if (m_enemyManager)
			{
				m_enemyManager->draw();
			}

			m_effectManager->draw();

			//記憶のかけら
			for (const auto& flagment : m_flagmentMemories)
			{
				flagment->draw();
			}
			//Jam::Util::GridRenderer::instance().draw();
		}

		BloomManager::getInstance().draw();
		m_inGameUIManager->draw();

		//フェードのDraw
		Jam::Presentation::FadeManager::instance().draw();
	}

	void GameScene::nextScene()
	{
		resetScene();
		changeScene(ToSceneString(SceneName::Result));
	}

	void GameScene::drawFadeIn(double t) const
	{
		draw();
		Jam::Presentation::Scenes::TransitionManager::Instance().drawFadeIn(
		Jam::Presentation::Scenes::TransitionType::RectSlide,
		t
		);
	}

	void GameScene::drawFadeOut(double t) const
	{
		draw();
		Jam::Presentation::Scenes::TransitionManager::Instance().drawFadeOut(
		Jam::Presentation::Scenes::TransitionType::RectSlide,
		t
		);
	}

	void GameScene::resetScene()
	{
		Jam::Infrastructure::IndependentObjectFactory::instance().clearAllObjects();
		AttackProcessor::getInstance().reset();
	}

	void GameScene::notifyCollisionEvents(const HashTable<P2ContactPair, P2Collision>& collisions)
	{
		HashSet<P2ContactPair> currentContacts;
		for (const auto& [pair, _] : collisions)
			currentContacts.insert(pair);

		for (const auto& [pair, col] : collisions)
		{
			if (!m_previousContacts.contains(pair))
			{
				auto a = findBodyByID(pair.a);
				auto b = findBodyByID(pair.b);
				if (a && b)
				{
					a->notifyCollisionEnter(b);
					b->notifyCollisionEnter(a);
				}
			}
			else
			{
				auto a = findBodyByID(pair.a);
				auto b = findBodyByID(pair.b);
				if (a && b)
				{
					a->notifyCollisionStay(b);
					b->notifyCollisionStay(a);
				}
			}
		}

		for (const auto& pair : m_previousContacts)
		{
			if (!currentContacts.contains(pair))
			{
				auto a = findBodyByID(pair.a);
				auto b = findBodyByID(pair.b);
				if (a && b)
				{
					a->notifyCollisionExit(b);
					b->notifyCollisionExit(a);
				}
			}
		}

		m_previousContacts = std::move(currentContacts);
	}

	std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody> GameScene::findBodyByID(P2BodyID id)
	{
		for (auto& body : m_physicsBodies)
			if (body->getBodyID() == id)
				return body;
		return nullptr;
	}
}
