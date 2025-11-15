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
#include "../../Infrastructure/IndependentObjectFactory.h"
#include "../../UseCase/IndependentObjectService.h"
#include "../../Presentation/IndependentObjectManager.h"
#include "../../Domain/GoalArea.h"
#include "SceneName.h"
#include "TransitionManager.h"

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
