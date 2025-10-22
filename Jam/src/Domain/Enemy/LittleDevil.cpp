#include "LittleDevil.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"

namespace Jam::Domain::Enemy
{
	LittleDevil::LittleDevil(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
	, Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
	{
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;
		aiList.emplace_back(AIType::Patrol, std::make_unique<PatrolAI>());

		setAIList(std::move(aiList));//setしたときにそのAIのEnterも入ります
		m_enemyType = EnemyType::LittleDevil;
	}

	void LittleDevil::update(double deltaTime)
	{
		if (!isAlive()) return;
		m_currentAI->update(*this, deltaTime);
		//moveRight();
	}

	void LittleDevil::onAIEvent(EnemyAIEvent e)
	{
		switch (e)
		{
		case EnemyAIEvent::PlayerFound:
			break;

		case EnemyAIEvent::PlayerLost:
			break;

		default:
			break;
		}
	}

	void LittleDevil::onPatrolEnter()
	{
	}

	void LittleDevil::onChaseEnter()
	{
	}


	void LittleDevil::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}

	void LittleDevil::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void LittleDevil::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}
}
