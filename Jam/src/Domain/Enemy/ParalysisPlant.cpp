#include "Domain/Enemy/ParalysisPlant.h"
#include "Infrastructure/FactoryServiceLocator.h"
#include "Infrastructure/IPhysicsBodyFactory.h"
#include "Infrastructure/PhysicsFilterManager.h"
#include "Infrastructure/IndependentObjectFactory.h"
#include "Domain/Enemy/ParalysisBullet.h"

namespace Jam::Domain::Enemy
{
	void ParalysisPlant::shootPoisonBullet(const Vec2& direction)
	{
		Vec2 startPos = m_body->getPosition() + direction * shotBulletDistance;

		auto physicsFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory();

		auto bulletBody = physicsFactory->createBody(
			startPos,
			size,
			s3d::P2BodyType::Dynamic,
			{0.1,0.0,1.0 },
			Jam::Domain::Physics::PhysicsShape::Circle
		);

		auto paralysisBullet = std::make_shared<Jam::Domain::Enemy::ParalysisBullet>(
			bulletBody,
			m_playerId,
			m_eventQueue,
			m_status.attackPower,
			5.0, // 弾の寿命
			3.0, // ホーミングする時間
			size,
			direction * speed
		);

		paralysisBullet->init();

		Jam::Infrastructure::IndependentObjectFactory::instance().registerObject(paralysisBullet);
	}
}
