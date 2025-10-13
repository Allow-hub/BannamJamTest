#pragma once
#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include "../PlayerManager.h"
#include "../../UseCase/PlayerService.h"
#include "../../Infrastructure/Siv3DInputManager.h"
#include "../../Infrastructure/Siv3DPhysicsBody.h"
#include "../../Infrastructure/PhysicsConverter.h"
#include "../EnemyManager.h"
#include "../../UseCase/EnemyFactory.h"
#include "../EnemyLoader.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	class GameScene : public App::Scene
	{
	private:
		P2World m_world;
		std::vector<std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody>> m_physicsBodies;
		double m_accumulatedTime = 0.0;
		std::shared_ptr<Domain::Player::Player> m_player;
		std::unique_ptr<Jam::UseCase::PlayerService> m_playerService;
		std::unique_ptr<Jam::Presentation::PlayerManager> m_playerManager;
		std::unique_ptr<Jam::Presentation::EnemyManager> m_enemyManager;
		std::unordered_map<Jam::UseCase::EnemyType, Jam::Domain::Enemy::EnemyStatus> m_enemyStatusTable;

		Jam::Infrastructure::Siv3DInputManager m_inputManager;
		std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody> m_ground;
		// 前フレームで接触していた相手の BodyID 一覧
		HashSet<P2ContactPair> m_previousContacts;
	public:
		GameScene(const InitData& init)
			: IScene{ init }, m_world({ 0, 980 }), m_inputManager()
		{
			auto stats = Jam::Infrastructure::Physics::LoadFromJSON(U"../Assets/Player/player_stats.json");

			// Player
			auto playerBody = std::make_shared<Infrastructure::Physics::Siv3DPhysicsBody>(
				m_world,
				Vec2{ 100, 300 },
				SizeF{ 50, 80 },
				s3d::P2BodyType::Dynamic,
				stats.physicsMaterial
			);
			m_player = std::make_shared<Domain::Player::Player>(playerBody);
			playerBody->setCollisionListener(m_player);

			m_physicsBodies.push_back(
				std::dynamic_pointer_cast<Infrastructure::Physics::Siv3DPhysicsBody>(
					m_player->getPhysicsBody()
				)
			);
			m_player->setSpeed(stats.moveSpeed);
			m_player->setJumpPower(stats.jumpPower);

			// PlayerManager / PlayerService
			m_playerManager = std::make_unique<Jam::Presentation::PlayerManager>(m_player);
			m_playerService = std::make_unique<Jam::UseCase::PlayerService>(
				m_player,
				m_inputManager,
				*m_playerManager
			);

			// Ground
			m_ground = std::make_shared<Infrastructure::Physics::Siv3DPhysicsBody>(
				m_world,
				Vec2{ 640, 700 },
				SizeF{ 1280, 40 },
				s3d::P2BodyType::Static,
				Jam::Domain::Physics::PhysicsMaterial{ 1.0,0.0,0.0 }
			);
			m_ground->setLayer(Jam::Domain::Physics::PhysicsLayer::Ground);
			m_physicsBodies.push_back(m_ground);

			// Enemyステータス読み込み
			Jam::Presentation::EnemyLoader::LoadEnemyStatusFromJSON(U"../Assets/Enemy/EnemyStatus.json", m_enemyStatusTable);

			// Factory に反映
			Jam::UseCase::EnemyFactory::SetStatusTable(m_enemyStatusTable);

			// EnemyManager初期化
			m_enemyManager = std::make_unique<Jam::Presentation::EnemyManager>();

			// PhysicsBody 作成
			auto body = std::make_shared<Infrastructure::Physics::Siv3DPhysicsBody>(
				m_world,
				Vec2{ 200, 200 },
				SizeF{ 64, 120 },
				s3d::P2BodyType::Dynamic,
				m_enemyStatusTable[Jam::UseCase::EnemyType::LittleDevil].physicsMaterial
			);

			// Enemy 生成
			auto enemy = Jam::UseCase::EnemyFactory::CreateEnemy(Jam::UseCase::EnemyType::LittleDevil, body);

			// CollisionListener 設定
			body->setCollisionListener(enemy);

			// ここで World に登録済みなので、m_physicsBodies に push するだけ
			m_physicsBodies.push_back(body);

			// Animator を読み込んで EnemyManager に登録
			int enemyID = m_enemyManager->AddEnemy(enemy, U"../Assets/Enemy/LittleDevil/littleDevil_animation.json");
			m_enemyManager->GetAnimator(enemyID).AddCondition({ { {U"isRunning", false} }, U"Idle", 0 });
			m_enemyManager->GetAnimator(enemyID).SetBool(U"isRunning", false);
		}

		void update() override
		{
			m_playerService->update(Scene::DeltaTime());
			m_playerManager->update();
			m_enemyManager->update(Scene::DeltaTime());

			constexpr double StepTime = 1.0 / 400.0;
			m_accumulatedTime += Scene::DeltaTime();

			while (StepTime <= m_accumulatedTime)
			{
				const auto collisions = m_world.getCollisions();
				notifyCollisionEvents(collisions);//当たり判定を持つオブジェクトにイベントを通知

				m_world.update(StepTime);
				m_accumulatedTime -= StepTime;
			}
		}

		void draw() const override
		{
			Scene::SetBackground(ColorF{ 0.9, 0.9, 1.0 });
			RectF{ 0, 680, 1280, 40 }.draw(Palette::Gray);

			m_playerManager->draw();
			m_enemyManager->draw();

			if (m_ground)
			{
				const auto t = m_ground->getTransform();
				RectF(t.position.x - 640, t.position.y - 20, 1280, 40).draw(Palette::Gray);
			}
		}

	private :
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
