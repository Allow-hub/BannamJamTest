#include "LittleDevil.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"

namespace Jam::Domain::Enemy
{
	LittleDevil::LittleDevil(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId)
		: EnemyBase(body,playerId)
	{
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;
		aiList.emplace_back(AIType::Patrol, std::make_unique<PatrolAI>());
		aiList.emplace_back(AIType::Chase, std::make_unique<ChaseAI>());

		setAIList(std::move(aiList));//setしたときにそのAIのEnterも入ります
	}

	void LittleDevil::update(double deltaTime)
	{
		if (!isAlive()) return;
		Print << U"Current AI: " + AITypeToString(getAIType());
		m_currentAI->update(*this, deltaTime);
		//moveRight();
	}

	void LittleDevil::onAIEvent(EnemyAIEvent e)
	{
		switch (e)
		{
		case EnemyAIEvent::PlayerFound:
			//changeAI(AIType::Chase);
			break;

		case EnemyAIEvent::PlayerLost:
			break;

		default:
			break;
		}
	}

	void LittleDevil::onPatrolEnter()
	{
		Print << U"Enter";
	}

	void LittleDevil::onChaseEnter()
	{
		Print << U"Chase";
	}


	void LittleDevil::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// TODO: 当たり判定で攻撃処理などを実装
	}

	void LittleDevil::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// 現状は何もしない
	}

	void LittleDevil::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// 現状は何もしない
	}
}
