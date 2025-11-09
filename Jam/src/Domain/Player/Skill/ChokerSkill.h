#pragma once
#include "../../Physics/ICollisionListener.h"
#include "../../Events/GameEvents.h"
#include "../../../Foundation/CoroutineUtil.h"

namespace Jam::Domain::Player
{
	class Player;
	struct PlayerStats;

	// チョーカー（グラップリングフック）- Playerの固有能力
	class ChokerSkill
		: public Jam::Domain::Physics::ICollisionListener
		, public std::enable_shared_from_this<ChokerSkill>
	{
	private:
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_targetEnemy;
		std::optional<s3d::P2DistanceJoint> m_joint;
		Player& m_player;
		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		Jam::Domain::Player::PlayerStats& m_playerStats;

		bool m_isActive = false;
		bool m_isFlying = false;
		bool m_isJointCreated = false;
		bool m_isHooked = false;

		Vec2 m_velocity = Vec2::Zero();
		Vec2 m_lastDir;
		Vec2 m_enemyImpluseDir = Vec2::Zero();
		const Vec2 createOffset = Vec2{ 50, -30 };
		Jam::Domain::Physics::PhysicsBodyID m_ownerId;

		const double m_maxFlyTime = 0.3;
		double m_flyTimer = 0.0;

		// クールダウン
		double m_cooldownTimer = 0.0;
		const double m_cooldownTime = 0.1;

		// フック挙動
		const double m_pullImpulse = 300.0;
		const double m_releaseImpulse = 300.0;
		const double m_hookedMoveSpeed = 3.0;

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_ground;
		Vec2 m_groundAnchorOffset = Vec2::Zero();

		double m_enemyHitFreezeTimer = 0.0;
		const double m_enemyHitFreezeDuration = 0.1;
		bool m_isInEnemyHitFreeze = false;
		bool m_isEnemySequenceActive = false;
		const double m_enemyJointShrinkSpeed = 0.85;

		void releaseJoint();
		void resetHook();
		Jam::Util::Task	delayReset();
		void hitGround();
		void hitEnemy(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> enemy);
		void createEnemyJoint();
		void finishEnemySequence();

	public:
		ChokerSkill(Jam::Domain::Events::GameEventQueue& eventQueue,
					Jam::Domain::Physics::PhysicsBodyID ownerId,
					Jam::Domain::Player::PlayerStats& stats,
					Player& player);

		void init();
		void use(const s3d::Vec2 position, bool facingRight);
		void useReleased(const s3d::Vec2 position, bool facingRight);
		void update(double deltaTime);
		void draw() const;
		bool needUpdate() const;

		// ICollisionListener
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		void onDeactivate();

		// ステータス情報
		bool isFlying() const;
		bool isOnCooldown() const;
		double getCooldownProgress() const;
		double getRemainingCooldown() const;
		double getHookedMoveSpeedMultiplier() const;

		enum class HookState
		{
			None,
			Idle,
			Flying,
			HookedGround,
			HookedEnemy
		};

		HookState m_hookState = HookState::Idle;
		~ChokerSkill();
	};
}
