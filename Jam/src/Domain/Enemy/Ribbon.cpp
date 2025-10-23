#include "Ribbon.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"

namespace Jam::Domain::Enemy
{
	Ribbon::Ribbon(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
	, Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
	{
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;
		aiList.emplace_back(AIType::Patrol, std::make_unique<PatrolAI>());

		setAIList(std::move(aiList));//setしたときにそのAIのEnterも入ります
		m_enemyType = EnemyType::Ribbon;
		m_body->setGravityScale(0);
	}

	void Ribbon::update(double deltaTime)
	{
		if (!isAlive()) return;
		m_currentAI->update(*this, deltaTime);
	}

	void Ribbon::onAIEvent(EnemyAIEvent e)
	{
		switch (e)
		{
		case EnemyAIEvent::PlayerFound:
		{
			changeAI(AIType::Chase);
		}break;

		case EnemyAIEvent::PlayerLost:
		{
			changeAI(AIType::Patrol);
		}break;

		default:
			break;
		}
	}

	void Ribbon::onPatrolEnter()
	{
	}

	void Ribbon::onPatrolUpdate(double deltaTime)
	{
		Vec2 plPos = getPlayerPos();
		Vec2 enePos = getPosition();
		Vec2 vector = plPos - enePos;
		//プレイヤーと敵の距離
		const double distance = vector.length();
		//探知範囲
		double foundRange = 300.0f;

		Print << distance;

		if (distance <= foundRange)
		{
			EnemyAIEvent::PlayerFound;
		}
	}

	void Ribbon::onChaseUpdate(double DeltaTime)
	{
		Vec2 plPos = getPlayerPos();
		Vec2 enePos = getPosition();
		Vec2 vector = plPos - enePos;
		//プレイヤーと敵の距離
		const double distance = vector.length();
		//探知範囲
		double attackRange = 200.0f;
		double lostRange = 1000.0f;

		Print << distance;

		if (distance <= attackRange)
		{
			changeAI(AIType::Attack);
		}
		if (distance >= lostRange)
		{
			changeAI(AIType::Patrol);
		}
	}

	void Ribbon::onChaseEnter()
	{

	}

	void Ribbon::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}

	void Ribbon::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void Ribbon::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}
}
