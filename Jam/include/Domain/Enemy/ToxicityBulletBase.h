#pragma once
#include <memory>
#include "Domain/Physics/IPhysicsBody.h"
#include "Domain/Events/GameEvents.h"
#include "Domain/Physics/ICollisionListener.h"
#include "Domain/IIndependentObject.h"
#include "Domain/Player/StatusAilment.h"

namespace Jam::Domain::Enemy
{
	class ToxicityBulletBase : public Jam::Domain::IIndependentObject
		, public Jam::Domain::Physics::ICollisionListener
		, public std::enable_shared_from_this<ToxicityBulletBase>
	{
	protected:
		Texture fbTex;

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		Jam::Domain::Physics::PhysicsBodyID m_playerId;

		double m_damage;
		double m_lifetime;
		double m_homingTime;
		double m_timer;
		double m_homingTimer;

		Vec2 m_size;
		Vec2 m_scaled = Vec2{ 0.0015,0.0015 };

		Vec2 m_Velocity;

		bool m_isHit = false;

		enum class HomingState
		{
			Homing,
			Straight,
		};

		HomingState m_state = HomingState::Homing;

		Jam::Domain::Player::StatusAilmentType m_statusType{ Jam::Domain::Player::StatusAilmentType::Poison };
		double m_statusDuration{ 0.0 };
		double m_statusPower{ 0.0 };
		double m_statusTickInterval{ 0.0 };

		virtual void setupStatusAilment() = 0;

	public:
		ToxicityBulletBase(
			std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& queue,
			double damage,
			double lifetime,
			double homingTime,
			Vec2 size,
			Vec2 Velocity
		);

		virtual ~ToxicityBulletBase();

		virtual void init();
		virtual void update(double deltaTime) override;
		virtual void draw() const override;

		virtual void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		virtual void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		virtual void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
	};
}
