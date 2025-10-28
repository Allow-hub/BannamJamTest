#include "Boss1_3.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"
#include "EnemyAI/AttackAI.h"

namespace Jam::Domain::Enemy
{
	Boss1_3::Boss1_3(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
	, Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
	{
		//std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;

		//setAIList(std::move(aiList));//setしたときにそのAIのEnterも入ります
		m_enemyType = EnemyType::Boss1_3;
		m_body->setGravityScale(0);
	}

	void Boss1_3::update(double deltaTime)
	{
		if (!isAlive()) return;
	}

	void Boss1_3::onAIEvent(EnemyAIEvent e)
	{
	}
	void Boss1_3::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}

	void Boss1_3::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void Boss1_3::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}
}
