#include "Ribbon.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"

namespace Jam::Domain::Enemy
{
	Ribbon::Ribbon(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId)
		: EnemyBase(body, playerId)
	{
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;
		aiList.emplace_back(AIType::Patrol, std::make_unique<PatrolAI>(getPosition(), 50));
		aiList.emplace_back(AIType::Chase, std::make_unique<ChaseAI>());

		setAIList(std::move(aiList));//setしたときにそのAIのEnterも入ります

		Print << U"Current AI: " + AITypeToString(getAIType());
	}

	void Ribbon::update(double deltaTime)
	{
		if (!isAlive()) return;
		m_currentAI->update(*this, deltaTime);
		//moveRight();
	}

	void Ribbon::onAIEvent(EnemyAIEvent e)
	{
		switch (e)
		{
		case EnemyAIEvent::PlayerFound:
			changeAI(AIType::Chase);
			break;

		case EnemyAIEvent::PlayerLost:
			changeAI(AIType::Patrol);
			break;

		default:
			break;
		}
	}

	void Ribbon::onPatrolEnter()
	{
		Print << U"Enter";
	}

	void Ribbon::onChaseEnter()
	{
		Print << U"Chase";
	}

	void Ribbon::onPatrolUpdate(double deltaTime)
	{
		Vec2 plPos = getPlayerPos();
		Vec2 enePos = getPosition();
		Vec2 vector = plPos - enePos;
		const double distance = vector.length();
		double foundRange = 150.0f;

		if(distance <= foundRange)
		{

		}
	}


	void Ribbon::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// TODO: 当たり判定で攻撃処理などを実装
	}

	void Ribbon::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// 現状は何もしない
	}

	void Ribbon::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// 現状は何もしない
	}
}
