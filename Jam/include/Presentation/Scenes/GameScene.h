#pragma once

// 標準ライブラリ
#include <Siv3D.hpp>
#include <HamFramework.hpp>

// Foundation層
#include "Foundation/CoreManager.h"

// Domain層
#include "Domain/FlagmentMemory.h"
#include "Domain/GoalArea.h"

// UseCase層
#include "UseCase/AttackProcessor.h"
#include "UseCase/CameraService.h"
#include "UseCase/EffectEvents.h"
#include "UseCase/EnemyFactory.h"
#include "UseCase/GameEventHandler.h"
#include "UseCase/IndependentObjectService.h"
#include "UseCase/PlayerService.h"
#include "UseCase/BlockService.h"

// Infrastructure層
#include "Infrastructure/Background/BackgroundLoader.h"
#include "Infrastructure/EnemyLoader.h"
#include "Infrastructure/FactoryServiceLocator.h"
#include "Infrastructure/GridRenderer.h"
#include "Infrastructure/IndependentObjectFactory.h"
#include "Infrastructure/PhysicsConverter.h"
#include "Infrastructure/PhysicsFilterManager.h"
#include "Infrastructure/Siv3DCursorUtil.h"
#include "Infrastructure/Siv3DInputManager.h"
#include "Infrastructure/Siv3DPhysicsBody.h"
#include "Infrastructure/Siv3DPhysicsBodyFactory.h"
#include "Infrastructure/StageLoader.h"
#include "Infrastructure/TextureLoader.h"

// Presentation層
#include "Presentation/AudioService.h"
#include "Presentation/CameraManager.h"
#include "Presentation/EffectManager.h"
#include "Presentation/EnemyManager.h"
#include "Presentation/FadeManager.h"
#include "Presentation/InGameUIManager.h"
#include "Presentation/IndependentObjectManager.h"
#include "Presentation/PlayerManager.h"
#include "Presentation/PostEffect/BloomManager.h"
#include "Presentation/Scenes/SceneName.h"
#include "Presentation/SettingManager.h"
#include "Presentation/Stage/BackgroundRenderer.h"
#include "Presentation/Stage/StageManager.h"

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
		std::unique_ptr<Jam::Presentation::Stage::StageManager> m_stageManager;
		std::vector<std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody>> m_stagePhysicsBodies;

		// Background用
		std::unique_ptr<Jam::Presentation::Background::BackgroundRenderer> m_backgroundRenderer;
		std::unique_ptr<Jam::Presentation::InGameUIManager> m_inGameUIManager;

		// Enemy用
		HashSet<P2ContactPair> m_previousContacts;

		std::vector<std::shared_ptr<Jam::Domain::FlagmentMemory>> m_flagmentMemories;

	public:
		explicit GameScene(const InitData& init);

		void update() override;
		void draw() const override;
		void nextScene();
		void drawFadeIn(double t) const override;
		void drawFadeOut(double t) const override;

	private:
		void resetScene();
		void notifyCollisionEvents(const HashTable<P2ContactPair, P2Collision>& collisions);
		std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody> findBodyByID(P2BodyID id);
	};
}
