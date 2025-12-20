#include "Domain/Enemy/ToxicPlant.h"
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
	ToxicPlant::ToxicPlant(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
	, Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
	{
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;
		aiList.emplace_back(AIType::Patrol, std::make_unique<PatrolAI>());
		aiList.emplace_back(AIType::Chase, std::make_unique<ChaseAI>());
		aiList.emplace_back(AIType::Attack, std::make_unique<AttackAI>());

		setAIList(std::move(aiList));	//setしたときにそのAIのEnterも入ります
		m_enemyType = EnemyType::ToxicPlant;
		m_body->setGravityScale(3.0);
	}

	void ToxicPlant::update(double deltaTime)
	{
		if (!isAlive()) return;
		m_currentAI->update(*this, deltaTime);
	}

	void ToxicPlant::onAIEvent(EnemyAIEvent e)
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

	void ToxicPlant::onAttackEnter()
	{
	}
	void ToxicPlant::onAttackUpdate(double deltaTime)
	{
		switch (attackState)
		{
		case Jam::Domain::Enemy::ToxicPlant::AttackState::IsAttackStart:
		{
			attackState = AttackState::IsBulletLaunch;
		}
		break;
		case Jam::Domain::Enemy::ToxicPlant::AttackState::IsBulletLaunch:
		{
			if (elapsedTime >= shotInterval)
			{
				Vec2 toPlayer = (getPlayerPos() - m_body->getPosition()).normalized();

				shootPoisonBullet(toPlayer);

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
		case Jam::Domain::Enemy::ToxicPlant::AttackState::IsAttackEnd:
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
	void ToxicPlant::onAttackExit()
	{
	}


	void ToxicPlant::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
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

	void ToxicPlant::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void ToxicPlant::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionExit(other);
	}

	void ToxicPlant::shootPoisonBullet(const Vec2& direction)
	{
		Vec2 startPos = m_body->getPosition() + direction * shotBulletDistance;

		auto physicsFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory();

		auto bulletBody = physicsFactory->createBody(
			startPos,
			size,
			s3d::P2BodyType::Dynamic,
			{ 0.1, 0.0, 1.0 },
			Jam::Domain::Physics::PhysicsShape::Circle
		);

		auto poisonBullet = std::make_shared<Jam::Domain::Enemy::PoisonBullet>(
			bulletBody,
			m_playerId,
			m_eventQueue,
			m_status.attackPower,
			5.0, // 弾の寿命
			3.0, // ホーミングする時間
			size,
			direction * speed
		);

		poisonBullet->init();

		Jam::Infrastructure::IndependentObjectFactory::instance().registerObject(poisonBullet);
	}
}
