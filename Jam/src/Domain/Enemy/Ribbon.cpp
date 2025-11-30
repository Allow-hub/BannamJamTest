#include "Domain/Enemy/Ribbon.h"
#include "Domain/Enemy/EnemyAI/PatrolAI.h"
#include "Domain/Enemy/EnemyAI/ChaseAI.h"
#include "Domain/Enemy/EnemyAI/AttackAI.h"

namespace Jam::Domain::Enemy
{
	Ribbon::Ribbon(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
	, Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
	{
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;
		aiList.emplace_back(AIType::Patrol, std::make_unique<PatrolAI>());
		aiList.emplace_back(AIType::Chase, std::make_unique<ChaseAI>());
		aiList.emplace_back(AIType::Attack, std::make_unique<AttackAI>());

		setAIList(std::move(aiList));//setしたときにそのAIのEnterも入ります
		m_enemyType = EnemyType::Ribbon;
		m_body->setGravityScale(1.5);
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
			changeAI(AIType::Chase);
			break;

		case EnemyAIEvent::PlayerLost:
			changeAI(AIType::Patrol);
			break;
		case EnemyAIEvent::ReachedGoal:
			changeAI(AIType::Attack);
			break;

		default:
			break;
		}
	}

	void Ribbon::onAttackEnter()
	{
		
	}

	void Ribbon::onAttackUpdate(double deltaTime)
	{
		switch (attackState)
		{
		case AttackState::AttackStart:
			Vec2 plPos = getPlayerPos();
			Vec2 enePos = getPosition();

			IsRight = (plPos.x > enePos.x);

			attackState = AttackState::WaitAttack;
			break;

		case AttackState::WaitAttack:
			if (AttackWaitTime >= 200)
			{
				//とりあえずイントのカウンターにしてます。後日調べて秒数計測の何かに置き換えます。
				attackState = AttackState::IsAttack;
				AttackWaitTime = 0;
			}
			else
			{
				AttackWaitTime++;
			}
			break;

		case AttackState::IsAttack:
			if (IsRight == true)
			{
				m_body->applyImpulse(Vec2{ 1500,0 });
			}
			else
			{
				m_body->applyImpulse(Vec2{ -1500,0 });
			}
			attackState = AttackState::EndAttack;
			break;

		case AttackState::EndAttack:
			if (AttackWaitTime >= 200)
			{
				AttackWaitTime = 0;
				attackState = AttackState::AttackStart;
				changeAI(AIType::Patrol);
			}
			else
			{
				AttackWaitTime++;
			}
			break;

		default:
			attackState = AttackState::AttackStart;
			break;
		}
	}

	void Ribbon::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
		switch (other->getLayer())
		{
		case Physics::PhysicsLayer::Player:
			m_eventQueue.push(Events::PlayerDamagedEvent{
				m_body->getID(),
				m_playerId,
				DamageInfo {
				m_status.attackPower,
				m_body->getPosition(),
				(getPlayerPos() - m_body->getPosition()).normalized(),
				true,
				false
				}
				,0.0
				,0.3
				,15.0
			});
			m_body->setVelocity({ 0,0 });
			break;
		}
	}

	void Ribbon::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void Ribbon::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}
}
