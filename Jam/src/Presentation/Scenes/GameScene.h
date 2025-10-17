#pragma once
#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include "../PlayerManager.h"
#include "../../UseCase/PlayerService.h"
#include "../../Infrastructure/Siv3DInputManager.h"
#include "../../Infrastructure/Siv3DPhysicsBody.h"
#include "../../Domain/Stage/NormalStage.h"
#include "../../Domain/Stage/MovingPlatformStage.h"
#include "../Stage/StageManager.h"
#include "../../Infrastructure/StageLoader.h"
#include "../../Infrastructure/PhysicsConverter.h"
#include "../EnemyManager.h"
#include "../../UseCase/EnemyFactory.h"
#include "../EnemyLoader.h"
#include "../CameraManager.h"
#include "../../UseCase/CameraService.h"
#include "../../UseCase/GameEventHandler.h"

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

		std::shared_ptr<Jam::Domain::Events::GameEventQueue> m_gameEventQueue;
		std::shared_ptr<Jam::UseCase::CameraEventQueue> m_cameraEventQueue;
		std::shared_ptr<Jam::UseCase::GameEventHandler> m_eventHandler;

		Jam::Infrastructure::Siv3DInputManager m_inputManager;
		std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody> m_ground;
		
		// Stage用
		std::unique_ptr<Jam::Presentation::Stage::StageManager> m_stageManager;
		
		// Enemy用
		HashSet<P2ContactPair> m_previousContacts;

	public:
		GameScene(const InitData& init)
			: IScene{ init },
			m_world({ 0, 980 }),//引数は重力
			m_inputManager()
		{
			// === Game内のイベント用クラスを初期化 ===
			m_gameEventQueue = std::make_shared<Jam::Domain::Events::GameEventQueue>();
			m_cameraEventQueue = std::make_shared<Jam::UseCase::CameraEventQueue>();
			m_eventHandler = std::make_shared<Jam::UseCase::GameEventHandler>(
				*m_gameEventQueue,*m_cameraEventQueue);

			// === Player 初期化 ===
			auto stats = Jam::Infrastructure::Physics::LoadFromJSON(U"../Assets/Player/player_stats.json");

			auto playerBody = std::make_shared<Infrastructure::Physics::Siv3DPhysicsBody>(
				m_world,
				Vec2{ 0, 0 },
				SizeF{ 50, 100 },
				s3d::P2BodyType::Dynamic,
				stats.physicsMaterial
			);

			// === Player 初期化 ===
			m_player = std::make_shared<Domain::Player::Player>(playerBody, *m_gameEventQueue);
			playerBody->setLayer(Jam::Domain::Physics::PhysicsLayer::Player);
			playerBody->setCollisionListener(m_player);

			m_physicsBodies.push_back(
				std::dynamic_pointer_cast<Infrastructure::Physics::Siv3DPhysicsBody>(
					m_player->getPhysicsBody()
				)
			);
			m_player->setSpeed(stats.moveSpeed);
			m_player->setJumpPower(stats.jumpPower);

			m_playerManager = std::make_unique<Jam::Presentation::PlayerManager>(m_player);
			m_playerService = std::make_unique<Jam::UseCase::PlayerService>(
				m_player,
				m_inputManager,
				*m_playerManager
			);

			// === Stage 初期化 ===
			m_stageManager = std::make_unique<Jam::Presentation::Stage::StageManager>();
			m_stageManager->initialize(m_world, m_physicsBodies);

			// === Camera 初期化 ===
			m_cameraManager = std::make_shared<Jam::Presentation::CameraManager>(
				m_player->getPosition()  // 初期位置をプレイヤー位置に合わせる
			);

			m_cameraService = std::make_shared<Jam::UseCase::CameraService>(
				*m_player,
				*m_cameraManager,
				*m_cameraEventQueue
			);

			// === Enemy 初期化 ===
			m_enemyFactory = std::make_unique<Jam::UseCase::EnemyFactory>();
			m_enemyManager = std::make_unique<Jam::Presentation::EnemyManager>();

			// 敵ステータスをJSONからロード
			std::unordered_map<Jam::UseCase::EnemyType, Jam::Domain::Enemy::EnemyStatus> enemyStatusTable;
			if (Jam::Presentation::EnemyLoader::LoadEnemyStatusFromJSON(
				U"../Assets/Enemy/enemy_stats.json",
				enemyStatusTable))
			{
				m_enemyFactory->setStatusTable(enemyStatusTable);
			}
			else
			{
				Console << U"[GameScene] ⚠ Failed to load enemy status";
			}

			// EnemyType を指定
			const auto enemyType = Jam::UseCase::EnemyType::LittleDevil;

			// ステータステーブルから該当データを探す
			auto it = enemyStatusTable.find(enemyType);
			if (it == enemyStatusTable.end())
			{
				Print << U"[GameScene] ⚠ Enemy status not found for type LittleDevil";
				return;
			}

			// ステータス取得
			const auto& status = it->second;

			// 敵を生成して配置
			auto enemyBody = std::make_shared<Infrastructure::Physics::Siv3DPhysicsBody>(
				m_world,
				Vec2{ 200, 0 },  // 初期位置
				status.colSize,   // サイズ
				s3d::P2BodyType::Dynamic,
				status.physicsMaterial
			);

			auto enemy = m_enemyFactory->createEnemy(
				Jam::UseCase::EnemyType::LittleDevil,
				enemyBody
			);

			if (enemy)
			{
				enemyBody->setCollisionListener(enemy);
				m_physicsBodies.push_back(enemyBody);
				// EnemyManagerに登録
				int enemyId = m_enemyManager->AddEnemy(enemy, U"../Assets/Enemy/LittleDevil/littleDevil_animation.json");
				m_enemyManager->getAnimator(enemyId).AddCondition({ { {U"isRunning", false} }, U"Idle", 0 });
				m_enemyManager->getAnimator(enemyId).SetBool(U"isRunning", false);
			}
			else
			{
				Console << U"[GameScene] ❌ Failed to create enemy";
			}
		}

		void update() override
		{
			m_playerService->update(Scene::DeltaTime());
			m_playerManager->update();

			// Stage更新
			if (m_stageManager)
			{
				m_stageManager->update();
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
			m_eventHandler->processEvents();//ゲーム内イベントを各クラスに通知

			// カメラの更新
			m_cameraService->update(Scene::DeltaTime());
		}

		void draw() const override
		{
			Scene::SetBackground(ColorF{ 0.9, 0.9, 1.0 });

			{
				const auto transformer = m_cameraManager->createTransformer();

				// Stage描画
				if (m_stageManager)
				{
					m_stageManager->draw();
				}

				// プレイヤーの描画
				m_playerManager->draw();

				// 敵の描画
				if (m_enemyManager)
				{
					m_enemyManager->draw();
				}
			}
		}

	private:

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
