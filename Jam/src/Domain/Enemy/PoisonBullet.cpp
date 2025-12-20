#include "Domain/Enemy/PoisonBullet.h"
#include "Infrastructure/FactoryServiceLocator.h"
#include "Infrastructure/IPhysicsBodyFactory.h"
#include "Infrastructure/PhysicsFilterManager.h"
#include "Presentation/AudioService.h"

using namespace Jam::Domain::Physics;
using namespace Jam::Infrastructure::Locator;

namespace Jam::Domain::Enemy
{
	PoisonBullet::PoisonBullet(
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
		m_timer =0.0;
		m_homingTimer =0.0;

		m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Dynamic);

		m_body->setGravityScale(0.0);

		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::EnemyWeapon);
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::Enemy);

		fbTex = Texture(Resource(U"Assets/Item/PoisonBullet_Poison.png"));
	}

	void PoisonBullet::init()
	{
		m_body->setCollisionListener(shared_from_this());
	}

	// 状態異常ごとにテクスチャを切り替える
	void PoisonBullet::setStatusAilment(
		Jam::Domain::Player::StatusAilmentType type,
		double duration,
		double power,
		double tickInterval)
	{
		m_statusType = type;
		m_statusDuration = duration;
		m_statusPower = power;
		m_statusTickInterval = tickInterval;
	}

	PoisonBullet::~PoisonBullet()
	{
		if (m_body)
		{
			auto factory = FactoryServiceLocator::instance().getPhysicsFactory();
			factory->removeBody(m_body->getID());
			m_body.reset();
		}
	}

	void PoisonBullet::update(double dt)
	{
		m_timer += dt;

		// ホーミング処理
		switch (m_state)
		{
		case HomingState::Homing:
		{
			m_homingTimer += dt;
			if (m_homingTimer >= m_homingTime)
			{
				// ホーミング終了して直進状態へ
				m_state = HomingState::Straight;
				break;
			}

			// ホーミング処理
			auto factory = FactoryServiceLocator::instance().getPhysicsFactory();
			auto playerBody = factory->getBody(m_playerId);

			if (playerBody)
			{
				Vec2 targetPos = playerBody->getPosition();
				Vec2 currentPos = m_body->getPosition();
				Vec2 toTarget = (targetPos - currentPos).normalized();

				// 現在の進行方向
				Vec2 currentDir = m_Velocity.normalized();

				double homingStrength =1.2 * dt;

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

		// 一定時間経過したら消滅
		if (m_timer >= m_lifetime)
		{
			m_isDead = true;
		}
	}

	void PoisonBullet::draw() const
	{
		if (!m_body) return;

		fbTex.scaled(m_size * m_scaled)
			.drawAt(m_body->getPosition());
	}
	void PoisonBullet::onCollisionEnter(std::shared_ptr<IPhysicsBody> other)
	{
		if (m_isHit) return;

		// プレイヤーに当たった場合
		if (other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Player)
		{
			// 状態異常を付与するイベントを発行
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
		// 壁や地面に当たった場合は消滅
		else if (other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Ground ||
				 other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Wall)
		{
			m_isHit = true;
			m_isDead = true;
		}
	}

	void PoisonBullet::onCollisionStay(std::shared_ptr<IPhysicsBody> other) {}
	void PoisonBullet::onCollisionExit(std::shared_ptr<IPhysicsBody> other) {}
}
