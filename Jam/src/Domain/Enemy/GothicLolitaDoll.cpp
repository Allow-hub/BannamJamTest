#include "GothicLolitaDoll.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"
#include "EnemyAI/AttackAI.h"

namespace Jam::Domain::Enemy
{
	GothicLolitaDoll::GothicLolitaDoll(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
	, Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
	{
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;
		aiList.emplace_back(AIType::Patrol, std::make_unique<PatrolAI>());
		aiList.emplace_back(AIType::Chase, std::make_unique<ChaseAI>());
		aiList.emplace_back(AIType::Attack, std::make_unique<AttackAI>());

		setAIList(std::move(aiList));//setしたときにそのAIのEnterも入ります
		m_enemyType = EnemyType::GothicLolitaDoll;
		m_body->setGravityScale(0);
	}

	void GothicLolitaDoll::update(double deltaTime)
	{
		if (!isAlive()) return;
		m_currentAI->update(*this, deltaTime);
	}

	void GothicLolitaDoll::onAIEvent(EnemyAIEvent e)
	{
		//switch (e)
		//{
		//case EnemyAIEvent::PlayerFound:
		//	break;

		//case EnemyAIEvent::PlayerLost:
		//	break;

		//default:
		//	break;
		//}
	}

	void GothicLolitaDoll::onPatrolEnter()
	{
	}

	void GothicLolitaDoll::onChaseEnter()
	{
	}


	void GothicLolitaDoll::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}

	void GothicLolitaDoll::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void GothicLolitaDoll::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}
}
