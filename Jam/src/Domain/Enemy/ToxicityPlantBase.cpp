#include "Domain/Enemy/ToxicityPlantBase.h"
#include "Domain/Enemy/EnemyAI/PatrolAI.h"
#include "Domain/Enemy/EnemyAI/ChaseAI.h"
#include "Domain/Enemy/EnemyAI/AttackAI.h"

namespace Jam::Domain::Enemy
{
	ToxicityPlantBase::ToxicityPlantBase(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
		Jam::Domain::Physics::PhysicsBodyID playerId,
		Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
	{
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;
		aiList.emplace_back(AIType::Patrol, std::make_unique<PatrolAI>());
		aiList.emplace_back(AIType::Chase, std::make_unique<ChaseAI>());
		aiList.emplace_back(AIType::Attack, std::make_unique<AttackAI>());

		setAIList(std::move(aiList));
		m_body->setGravityScale(3.0);
	}

	void ToxicityPlantBase::update(double deltaTime)
	{
		if (!isAlive()) return;
		m_currentAI->update(*this, deltaTime);
	}

	void ToxicityPlantBase::onAIEvent(EnemyAIEvent e)
	{
		switch (e)
		{
		case Jam::Domain::Enemy::EnemyAIEvent::PlayerFound:
			changeAI(AIType::Chase);
			break;
		case Jam::Domain::Enemy::EnemyAIEvent::PlayerLost:
			changeAI(AIType::Patrol);
			break;
		case Jam::Domain::Enemy::EnemyAIEvent::ReachedGoal:
			changeAI(AIType::Attack);
			break;
		default:
			break;
		}
	}

	void ToxicityPlantBase::onAttackEnter()
	{
	}

	void ToxicityPlantBase::onAttackUpdate(double)
	{
		switch (attackState)
		{
		case AttackState::IsAttackStart:
		{
			attackState = AttackState::IsBulletLaunch;
		}
		break;
		case AttackState::IsBulletLaunch:
		{
			if (elapsedTime >= shotInterval)
			{
				Vec2 toPlayer = (getPlayerPos() - m_body->getPosition()).normalized();

				shootBullet(toPlayer);

				shotCount++;
				elapsedTime = 0;
			}
			else
			{
				elapsedTime++;
			}

			if (shotCount >= maxShotCount)
			{
				shotCount = 0;
				elapsedTime = 0;
				attackState = AttackState::IsAttackEnd;
			}
		}
		break;
		case AttackState::IsAttackEnd:
		{
			if (elapsedTime >= attackCooldown)
			{
				elapsedTime = 0;
				attackState = AttackState::IsAttackStart;
				changeAI(AIType::Chase);
			}
			else
			{
				elapsedTime++;
			}
		}
		break;
		default:
			break;
		}
	}

	void ToxicityPlantBase::onAttackExit()
	{
	}

	void ToxicityPlantBase::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
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

	void ToxicityPlantBase::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}

	void ToxicityPlantBase::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionExit(other);
	}
}
