#include "Domain/Enemy/PoisonPlant.h"
#include "Domain/Enemy/EnemyAI/PatrolAI.h"
#include "Domain/Enemy/EnemyAI/ChaseAI.h"
#include "Domain/Enemy/EnemyAI/AttackAI.h"

#include "Infrastructure/FactoryServiceLocator.h"
#include "Infrastructure/IPhysicsBodyFactory.h"
#include "Infrastructure/PhysicsFilterManager.h"
#include "Infrastructure/IndependentObjectFactory.h"
#include "Domain/Enemy/PoisonBullet.h"

namespace Jam::Domain::Enemy
{
	PoisonPlant::PoisonPlant(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
		Jam::Domain::Physics::PhysicsBodyID playerId,
		Jam::Domain::Events::GameEventQueue& eventQueue)
		: ToxicityPlantBase(body, playerId, eventQueue)
	{
		m_enemyType = EnemyType::PoisonPlant;
	}

	void PoisonPlant::shootBullet(const Vec2& direction)
	{
		Vec2 startPos = m_body->getPosition() + direction * shotBulletDistance;

		auto physicsFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory();

		auto bulletBody = physicsFactory->createBody(
			startPos,
			size,
			s3d::P2BodyType::Dynamic,
			{ 0.1,0.0,1.0 },
			Jam::Domain::Physics::PhysicsShape::Circle
		);

		auto poisonBullet = std::make_shared<Jam::Domain::Enemy::PoisonBullet>(
			bulletBody,
			m_playerId,
			m_eventQueue,
			m_status.attackPower,
			5.0,
			3.0,
			size,
			direction * speed
		);

		poisonBullet->init();

		Jam::Infrastructure::IndependentObjectFactory::instance().registerObject(poisonBullet);
	}
}
