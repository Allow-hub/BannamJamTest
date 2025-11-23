#include "GothicLolitaDoll.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"
#include "EnemyAI/AttackAI.h"

#include "../../Infrastructure/FactoryServiceLocator.h"
#include "../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../Infrastructure/PhysicsFilterManager.h"
#include "../../Infrastructure/IndependentObjectFactory.h"
#include "FireBall.h"

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

	void GothicLolitaDoll::onAttackEnter()
	{
	}
	void GothicLolitaDoll::onAttackUpdate(double deltaTime)
	{
		// プレイヤーへの方向を計算
		Vec2 playerPos = getPlayerPos();
		Vec2 myPos = m_body->getPosition();
		Vec2 direction = (playerPos - myPos).normalized();

		// 速度(スカラ)を決める
		double speed = 10.0;

		// 渡すためのベクトルを作成 (方向 × 速さ)
		Vec2 throwVelocity = direction * speed;
		switch (m_attackStatus)
		{
		case Jam::Domain::Enemy::GothicLolitaDoll::m_AttackStatus::IsAttackStart:
		{
			// 次の状態へ移行
			m_attackStatus = m_AttackStatus::IsFireBallLaunch;
		}
		break;
		case Jam::Domain::Enemy::GothicLolitaDoll::m_AttackStatus::IsFireBallLaunch:
		{
			if (AttackWaitTime >= 90)
			{
				Vec2 toPlayer = (getPlayerPos() - m_body->getPosition()).normalized();

				shootFireball(toPlayer);

				// カウントアップとタイマーリセット
				m_shotCount++;
				AttackWaitTime = 0;
			}
			else
			{
				AttackWaitTime++;
			}

			// 3発撃ち終わったら終了
			if (m_shotCount >= 3)
			{
				m_shotCount = 0;
				AttackWaitTime = 0;
				m_attackStatus = m_AttackStatus::IsAttackEnd;
			}
		}
		break;
		case Jam::Domain::Enemy::GothicLolitaDoll::m_AttackStatus::IsAttackEnd:
		{
			if (AttackWaitTime >= 800)
			{
				AttackWaitTime = 0;
				m_attackStatus = m_AttackStatus::IsAttackStart;
				changeAI(AIType::Chase);
			}
			else
			{
				AttackWaitTime++;
			}
		}
		break;
		default:
			break;
		}
	}
	void GothicLolitaDoll::onAttackExit()
	{
	}


	void GothicLolitaDoll::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
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

	void GothicLolitaDoll::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void GothicLolitaDoll::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionExit(other);
	}

	void GothicLolitaDoll::shootFireball(const Vec2& direction)
	{
		Vec2 startPos = m_body->getPosition() + direction * 40.0; // 少し前から出す
		Vec2 size = { 40, 40 };
		double speed = 400.0; // 弾速

		auto physicsFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory();

		auto fireballBody = physicsFactory->createBody(
			startPos,
			size,
			s3d::P2BodyType::Dynamic,
			{ 0.1, 0.0, 1.0 },
			Jam::Domain::Physics::PhysicsShape::Circle
		);

		// コンストラクタに速度ベクトル（方向×速さ）を渡す
		auto fireball = std::make_shared<Jam::Domain::Enemy::Fireball>(
			fireballBody,
			m_playerId,
			m_eventQueue,
			m_status.attackPower,
			3.0,
			size,
			direction * speed
		);
		fireball->init();

		Jam::Infrastructure::IndependentObjectFactory::instance().registerObject(fireball);
	}
}
