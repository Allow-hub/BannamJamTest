#include "Ribbon.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"
#include "EnemyAI/AttackAI.h"

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
		m_body->setGravityScale(1);
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
		//攻撃開始時に、プレイヤーがどちらの方向にいるのか特定する
		case AttackState::AttackStart:
			Vec2 plPos = getPlayerPos();
			Vec2 enePos = getPosition();

			IsRight = (plPos.x > enePos.x);
			attackTimer.start();
			attackState = AttackState::WaitAttack;
			break;

		//プレイヤーが有効射程に入ってから攻撃を出すまでの時間
		case AttackState::WaitAttack:
		{
			if (attackTimer.reachedZero())
			{
				attackTimer.reset();
			}
		}break;

		//攻撃処理
		case AttackState::IsAttack:
			if (IsRight == true)
			{
				m_body->applyImpulse(Vec2{ 1000,0 });
			}
			else
			{
				m_body->applyImpulse(Vec2{ -1000,0 });
			}
			attackTimer.start();
			attackState = AttackState::EndAttack;


		//攻撃終了後の待機時間と、攻撃終了後にAIを変更する処理
		case AttackState::EndAttack:
		{
			if (attackTimer.reachedZero())
			{
				attackTimer.reset();
				attackState = AttackState::AttackStart;
				changeAI(AIType::Patrol);
			}
		}break;

		default:
			attackState = AttackState::AttackStart;
			break;
		}
		//現在は、WaitAttackとEndAttackの時のみTimerを使用しているため、AttackStartとIsAttackでタイマーをスタートさせる必要があります。
		//わかりにくいのでもっとちゃんとしたステート管理の処理に置き換えたいと考えています。
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
			break;
		}
	}

	void Ribbon::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void Ribbon::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}
}
