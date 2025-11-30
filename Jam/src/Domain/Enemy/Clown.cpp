#include "Domain/Enemy/Clown.h"
#include "Domain/Enemy/EnemyAI/PatrolAI.h"
#include "Domain/Enemy/EnemyAI/ChaseAI.h"
#include "Domain/Enemy/EnemyAI/AttackAI.h"
#include "Domain/Enemy/EnemyAI/RunAwayAI.h"
#include "Infrastructure/FactoryServiceLocator.h"
#include "Infrastructure/IPhysicsBodyFactory.h"
#include "Infrastructure/PhysicsFilterManager.h"
#include "Infrastructure/IndependentObjectFactory.h"
#include "Domain/Enemy/Bomb.h"

namespace Jam::Domain::Enemy
{
	Clown::Clown(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
	, Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
	{
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;
		aiList.emplace_back(AIType::Patrol, std::make_unique<PatrolAI>());
		aiList.emplace_back(AIType::Chase, std::make_unique<ChaseAI>());
		aiList.emplace_back(AIType::Attack, std::make_unique<AttackAI>());
		aiList.emplace_back(AIType::RunAway, std::make_unique<RunAwayAI>());

		setAIList(std::move(aiList));//setしたときにそのAIのEnterも入ります
		m_enemyType = EnemyType::Clown;
		m_body->setGravityScale(1.5);
	}

	void Clown::update(double deltaTime)
	{
		if (!isAlive()) return;
		m_currentAI->update(*this, deltaTime);
	}

	void Clown::onAIEvent(EnemyAIEvent e)
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

	void Clown::onAttackEnter()
	{
		Vec2 playerPos = getPlayerPos();
		bool isPlayerOnRight = playerPos.x > m_body->getPosition().x;
		double direction = isPlayerOnRight ? 1.0 : -1.0;

		auto offset = Vec2{ m_offset.x * direction, m_offset.y };
		auto pos = m_body->getPosition() + offset;

		Vec2 size = { 50, 50 };

		auto physicsFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory();
		auto bombBody = physicsFactory->createBody(
			pos, size,
			s3d::P2BodyType::Dynamic,
			{ 0.2, 0.3, 1.0 },
			Jam::Domain::Physics::PhysicsShape::Circle
		);

		auto bomb = std::make_shared<Jam::Domain::Enemy::Bomb>(
			bombBody, m_playerId, m_eventQueue, m_status.attackPower,
			size.x, m_explosionDelay, size
		);
		bomb->init();

		// 向いている方向に放り投げる
		Vec2 impulse = Vec2{ m_throwForce.x * direction,  m_throwForce.y }; // 前方＋上方向
		bombBody->applyImpulse(impulse);

		Jam::Infrastructure::IndependentObjectFactory::instance().registerObject(bomb);
		changeAI(AIType::RunAway);
	}


	void Clown::onAttackUpdate(double deltaTime)
	{
	}
	void Clown::onAttackExit()
	{
	}

	void Clown::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
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
			break;
		}
	}

	void Clown::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void Clown::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}
}
