#include "ShockWave.h"
#include "../../Infrastructure/FactoryServiceLocator.h"
#include "../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../Infrastructure/PhysicsFilterManager.h"

using namespace Jam::Domain::Physics;
using namespace Jam::Infrastructure::Locator;

namespace Jam::Domain::Enemy
{
	ShockWave::ShockWave(const Vec2& pos, double duration, Vec2 startRect, Vec2 endRect, Jam::Domain::Physics::PhysicsBodyID playerId, Jam::Domain::Events::GameEventQueue& queue)
		: m_duration(duration)
		, m_startRect(startRect)
		, m_endRect(endRect)
		, m_playerId(playerId)
		, m_eventQueue(queue)
		, m_lastRegenTime(0.0)
	{
		m_body = FactoryServiceLocator::instance()
			.getPhysicsFactory()->createRectSensor(pos, m_startRect);
		m_body->setLayer(PhysicsLayer::Enemy);
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::EnemyWeapon);
		m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Static);
		m_body->setGravityScale(0);
		m_hittedPlayer = false;
	}

	void ShockWave::update(double dt)
	{
		m_timer += dt;

		// 寿命終了
		if (m_timer >= m_duration)
		{
			m_finished = true;
			m_body.reset();
			return;
		}

		double t = m_timer / (m_duration * 0.5);
		Vec2 currentRect = Math::Lerp(m_startRect, m_endRect, t);

		// 一定間隔ごとにのみ再生成（0.1秒ごと）
		if (m_timer - m_lastRegenTime >= 0.1)
		{
			m_lastRegenTime = m_timer;
			auto pos = m_body->getPosition();

			// 古いボディを破棄して新しいものを生成
			m_body.reset();
			m_body = FactoryServiceLocator::instance()
				.getPhysicsFactory()->createRectSensor(pos, currentRect);
			m_body->setCollisionListener(shared_from_this());
			m_body->setLayer(PhysicsLayer::Enemy);
			m_body->setFilter(Jam::Infrastructure::PhysicsFilter::EnemyWeapon);
			m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Static);
			m_body->setGravityScale(0);
		}
	}

	void ShockWave::draw() const
	{
		if (!m_body) return;

		double t = m_timer / (m_duration * 0.5);
		Vec2 currentRect = Math::Lerp(m_startRect, m_endRect, t);
		RectF rect(
			m_body->getPosition() - currentRect / 2, // 左上座標
			currentRect
		);
		rect.drawFrame(4.0, ColorF(0.3, 0.6, 1.0, 0.4));
	}


	void ShockWave::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		switch (other->getLayer())
		{
		case Jam::Domain::Physics::PhysicsLayer::Player:
			if (m_hittedPlayer) return;//一回のみ
			m_eventQueue.push(Events::PlayerDamagedEvent{
				m_body->getID(),
				m_playerId,
				DamageInfo {
				m_attackPower,
				m_body->getPosition(),
				(Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory()->getBody(m_playerId)->getPosition()
					- m_body->getPosition()).normalized(),
				true,
				false
				}
				,0.0
				,0.3
				,15.0
			});
			m_hittedPlayer = true;
			break;
		}
	}

	void ShockWave::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
	}

	void ShockWave::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
	}
}
