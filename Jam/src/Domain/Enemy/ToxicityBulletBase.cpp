#include "Domain/Enemy/ToxicityBulletBase.h"
#include "Infrastructure/FactoryServiceLocator.h"
#include "Infrastructure/IPhysicsBodyFactory.h"
#include "Infrastructure/PhysicsFilterManager.h"

using namespace Jam::Domain::Physics;
using namespace Jam::Infrastructure::Locator;

namespace Jam::Domain::Enemy
{
	ToxicityBulletBase::ToxicityBulletBase(
		std::shared_ptr<IPhysicsBody> body,
		PhysicsBodyID playerId,
		Jam::Domain::Events::GameEventQueue& queue,
		double damage,
		double lifetime,
		double homingTime,
		Vec2 size,
		Vec2 Velocity
	)
		: m_body(body)
		, m_playerId(playerId)
		, m_eventQueue(queue)
		, m_damage(damage)
		, m_lifetime(lifetime)
		, m_homingTime(homingTime)
		, m_size(size)
		, m_Velocity(Velocity)
	{
		m_timer = 0.0;
		m_homingTimer = 0.0;

		m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Dynamic);
		m_body->setGravityScale(0.0);
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::EnemyWeapon);
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::Enemy);
	}

	ToxicityBulletBase::~ToxicityBulletBase()
	{
		if (m_body)
		{
			auto factory = FactoryServiceLocator::instance().getPhysicsFactory();
			factory->removeBody(m_body->getID());
			m_body.reset();
		}
	}

	void ToxicityBulletBase::init()
	{
		m_body->setCollisionListener(shared_from_this());
	}

	void ToxicityBulletBase::update(double dt)
	{
		m_timer += dt;

		switch (m_state)
		{
		case HomingState::Homing:
		{
			m_homingTimer += dt;
			if (m_homingTimer >= m_homingTime)
			{
				m_state = HomingState::Straight;
				break;
			}

			auto factory = FactoryServiceLocator::instance().getPhysicsFactory();
			auto playerBody = factory->getBody(m_playerId);

			if (playerBody)
			{
				Vec2 targetPos = playerBody->getPosition();
				Vec2 currentPos = m_body->getPosition();
				Vec2 toTarget = (targetPos - currentPos).normalized();

				Vec2 currentDir = m_Velocity.normalized();

				double homingStrength = 1.2 * dt;

				Vec2 newDir = (currentDir + toTarget * homingStrength).normalized();

				double speed = m_Velocity.length();
				m_Velocity = newDir * speed;
			}
		}
		break;

		case HomingState::Straight:
			break;
		}

		m_body->setVelocity(m_Velocity);

		if (m_timer >= m_lifetime)
		{
			m_isDead = true;
		}
	}

	void ToxicityBulletBase::draw() const
	{
		if (!m_body) return;

		fbTex.scaled(m_size * m_scaled)
			.drawAt(m_body->getPosition());
	}

	void ToxicityBulletBase::onCollisionEnter(std::shared_ptr<IPhysicsBody> other)
	{
		if (m_isHit) return;

		if (other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Player)
		{
			m_eventQueue.push(Events::PlayerStatusAilmentEvent{
				m_body->getID(),
				m_playerId,
				m_statusType,
				m_statusDuration,
				m_statusPower,
				m_statusTickInterval
				});

			m_isHit = true;
			m_isDead = true;
		}
		else if (other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Ground ||
			other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Wall)
		{
			m_isHit = true;
			m_isDead = true;
		}
	}

	void ToxicityBulletBase::onCollisionStay(std::shared_ptr<IPhysicsBody> other) {}
	void ToxicityBulletBase::onCollisionExit(std::shared_ptr<IPhysicsBody> other) {}
}
