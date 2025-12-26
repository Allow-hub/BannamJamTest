#pragma once
#include "Domain/Enemy/EnemyBase.h"
#include "Domain/Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	class ToxicityPlantBase : public EnemyBase
	{
	public:
		explicit ToxicityPlantBase(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& eventQueue);

		virtual ~ToxicityPlantBase() = default;

		void update(double deltaTime) override;
		void draw() const override {};

		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		void onAIEvent(EnemyAIEvent e) override;

	protected:
		void onAttackEnter() override;
		void onAttackUpdate(double deltaTime) override;
		void onAttackExit() override;

		virtual void shootBullet(const Vec2& direction) = 0;

		const int attackCooldown = 500;
		const int shotInterval = 60;
		const int maxShotCount = 1;

		const float shotBulletDistance = 50.0f;
		const Vec2 size = { 40,40 };
		const double speed = 150.0;

		int elapsedTime = 0;
		int shotCount = 0;

		enum class AttackState
		{
			IsAttackStart = 0,
			IsBulletLaunch,
			IsAttackEnd,
		};
		AttackState attackState = AttackState::IsAttackStart;
	};
}
