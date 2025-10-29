#pragma once
#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include "../../Foundation/CoreManager.h"
#include "../PlayerManager.h"
#include "../../UseCase/PlayerService.h"
#include "../../Infrastructure/Siv3DInputManager.h"
#include "../../Infrastructure/Siv3DPhysicsBody.h"
#include "../../Infrastructure/StageLoader.h"
#include "../Stage/StageManager.h"
#include "../../UseCase/StageService.h"
#include "../../Infrastructure/PhysicsConverter.h"
#include "../EnemyManager.h"
#include "../../UseCase/EnemyFactory.h"
#include "../../Infrastructure/EnemyLoader.h"
#include "../CameraManager.h"
#include "../../UseCase/CameraService.h"
#include "../../UseCase/GameEventHandler.h"
#include "../../Infrastructure/Siv3DPhysicsBodyFactory.h"
#include "../../Infrastructure/FactoryServiceLocator.h"
#include "../../Infrastructure/Siv3DCursorUtil.h"
#include "../../Presentation/AudioService.h"
#include "../../Infrastructure/PhysicsFilterManager.h"
#include "../../Infrastructure/TextureLoader.h"
#include "../Stage/BackgroundRenderer.h"
#include "../../Infrastructure/Background/BackgroundLoader.h"
#include "../../UseCase/AttackProcessor.h"
#include "../../Infrastructure/GridRenderer.h"
#include "../../UseCase/EffectEvents.h"
#include "../../Presentation/EffectManager.h"
#include "../../Presentation/FadeManager.h"
#include "../PostEffect/BloomManager.h"
#include "../SettingManager.h"
#include "../../Domain/FlagmentMemory.h"
#include "../InGameUIManager.h"


namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	class GameScene : public App::Scene
	{
	private:
		P2World m_world;
		std::vector<std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody>> m_physicsBodies;
		double m_accumulatedTime = 0.0;

		// Player
		std::shared_ptr<Domain::Player::Player> m_player;
		std::unique_ptr<Jam::UseCase::PlayerService> m_playerService;
		std::unique_ptr<Jam::Presentation::PlayerManager> m_playerManager;

		std::unique_ptr<Jam::Presentation::EnemyManager> m_enemyManager;
		std::unique_ptr<Jam::UseCase::EnemyFactory> m_enemyFactory;

		std::shared_ptr<Jam::Presentation::CameraManager> m_cameraManager;
		std::shared_ptr<Jam::UseCase::CameraService> m_cameraService;
		std::unique_ptr<Jam::Presentation::EffectManager> m_effectManager;


		std::shared_ptr<Jam::Domain::Events::GameEventQueue> m_gameEventQueue;
		std::shared_ptr<Jam::UseCase::CameraEventQueue> m_cameraEventQueue;
		std::shared_ptr<Jam::UseCase::EffectEventQueue> m_effectEventQueue;
		std::shared_ptr<Jam::UseCase::GameEventHandler> m_eventHandler;

		Jam::Infrastructure::Siv3DInputManager m_inputManager;
		std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody> m_ground;

		// Stage用
		std::shared_ptr<Jam::UseCase::StageService> m_stageService;
		std::unique_ptr<Jam::Presentation::Stage::StageManager> m_stageManager;		// Stage用物理ボディ管理
		std::vector<std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody>> m_stagePhysicsBodies;

		// Background用
		std::unique_ptr<Jam::Presentation::Background::BackgroundRenderer> m_backgroundRenderer;
		std::unique_ptr<Jam::Presentation::InGameUIManager> m_inGameUIManager;

		// Enemy用
		HashSet<P2ContactPair> m_previousContacts;

		std::vector<std::shared_ptr<Jam::Domain::FlagmentMemory>> m_flagmentMemories;

	public:
		GameScene(const InitData& init)
			: IScene{ init },
			m_world({ 0, 980 }),//引数は重力
			m_inputManager()
		{
			Jam::Presentation::AudioService::get().play(Jam::Presentation::AudioService::Sound::BGM_Title, true);
			//デバッグ用
			Jam::Foundation::CoreManager::Instance().stageInfo.stageName = Jam::Foundation::StageName::Stage1_3; // CoreManagerのenumに追加が必要

			auto& core = Jam::Foundation::CoreManager::Instance();
			core.reset();
			String stageName = Jam::Foundation::CoreManager::stageNameToString(core.stageInfo.stageName);//ステージ名
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

			m_eventHandler = std::make_shared<Jam::UseCase::GameEventHandler>(
				*m_gameEventQueue, *m_cameraEventQueue, *m_effectEventQueue,
				[this]() { this->nextScene(); });


			m_effectManager = std::make_unique<Jam::Presentation::EffectManager>(*m_effectEventQueue);

			// === Player 初期化 ===
			auto stats = Jam::Infrastructure::Physics::LoadFromJSON(U"../Assets/Player/player_stats.json");

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
			m_stageService->initialize(stageName + U".json", bodyFactory);

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
			if (Jam::Infrastructure::EnemyLoader::LoadEnemyStatusFromJSON(
				U"../Assets/Enemy/enemy_stats.json",
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

			// ステージ用敵配置 JSON をロードして敵を生成
			if (!Jam::Infrastructure::EnemyLoader::loadEnemyForStageFromJSON(
				U"../Assets/Enemy/enemy_" + stageName + U".json",
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

			// JSONから背景データを読み込み
			Array<Jam::Domain::Background::BackgroundObject> backgroundObjects;
			if (Jam::Infrastructure::Background::BackgroundLoader::loadBackgroundFromFile(U"background.json", backgroundObjects)) {
				m_backgroundRenderer->setBackgroundObjects(backgroundObjects);
			}
			else {
				Print << U"[GameScene] ⚠️ Failed to load background JSON, using fallback";
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

			Jam::Util::GridRenderer::GridConfig config;
			config.gridSize = 100.0;
			config.fontSize = 16;
			Jam::Util::GridRenderer::instance().setConfig(config);
			// 元画像を読み込む
			Image originalImage(U"../Assets/Cursor/cursor_yellow.png");
			originalImage = originalImage.scaled(64, 64);
			// カーソル登録
			Jam::Infrastructure::CursorUtil::instance().registerCustomCursor(
				U"Cross",
				originalImage,
				Point{ originalImage.width() / 2, originalImage.height() / 2 } // hotSpotは中央
			);
			BloomManager::getInstance().setIntensities(0.03, 0.06, 0.1, 0.3);
			BloomManager::getInstance().setVignette(0.3, 0.5);
		}

		void update() override
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
		}

		void draw() const override
		{
			Scene::SetBackground(ColorF{ 0.9, 0.9, 1.0 });

			{
				if (Jam::Foundation::CoreManager::Instance().getPause())
				{
					Jam::Presentation::SettingManager::Instance().draw();
					return;
				}
				//const auto t = Jam::Presentation::BloomManager::getInstance().getRenderTarget();
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
			m_inGameUIManager->draw();

			//BloomManager::getInstance().draw();
			//フェードのDraw
			Jam::Presentation::FadeManager::instance().draw();
		}

		void nextScene()
		{
			resetScene();
			changeScene(ToSceneString(SceneName::Result));
		}

	private:

		void resetScene()
		{
			Jam::UseCase::AttackProcessor::getInstance().reset();
		}

		void notifyCollisionEvents(const HashTable<P2ContactPair, P2Collision>& collisions)
		{
			HashSet<P2ContactPair> currentContacts;
			for (const auto& [pair, _] : collisions)
				currentContacts.insert(pair);

			// Enter = 新しく発生した接触
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
			}

			// Exit = 前フレームあって今ない
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

		std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody> findBodyByID(P2BodyID id)
		{
			for (auto& body : m_physicsBodies)
				if (body->getBodyID() == id)
					return body;
			return nullptr;
		}
	};
}
