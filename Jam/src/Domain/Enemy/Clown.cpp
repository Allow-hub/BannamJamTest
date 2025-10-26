#include "Clown.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"
#include "EnemyAI/AttackAI.h"
#include "EnemyAI/RunAwayAI.h"

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
		m_body->setGravityScale(1);
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
		case EnemyAIEvent::PlayerFound:
			Print << U"ChangeAi2Chase";
			changeAI(AIType::Chase);
			break;

		case EnemyAIEvent::PlayerLost:
			Print << U"ChangeAi2Patrol";
			changeAI(AIType::Patrol);
			break;
		case EnemyAIEvent::ReachedGoal:
			Print << U"ChangeAi2Attack";
			changeAI(AIType::Attack);
			break;

		default:
			break;
		}
	}
	void Clown::onChaseEnter()
	{

	}

	void Clown::onChaseUpdate(double deltaTime)
	{

	}

	void Clown::onAttackEnter()
	{

	}

	void Clown::onAttackUpdate(double deltaTime)
	{
		switch (attackState)
		{
		//攻撃開始時に、プレイヤーがどちらの方向にいるのか特定する
		case AttackState::AttackStart:
		{
			//ステート切り替えの際にタイマーを開始
			attackTimer.start();
			attackState = AttackState::WaitAttack;
		}break;

		//プレイヤーが有効射程に入ってから攻撃を出すまでの時間
		case AttackState::WaitAttack:
		{
			if (attackTimer.reachedZero())
			{
				attackTimer.reset();
				attackState = AttackState::IsAttack;
			}
		}break;

		//攻撃処理
		case AttackState::IsAttack:
		{
			Vec2 plPos = getPlayerPos();
			Vec2 enePos = getPosition();
			//ステート切り替えの際にタイマーを開始
			attackTimer.start();
			attackState = AttackState::EndAttack;
		}break;

		//攻撃終了後の待機時間と、攻撃終了後にAIを変更する処理
		case AttackState::EndAttack:
		{
			if (attackTimer.reachedZero())
			{
				attackTimer.reset();
				attackState = AttackState::AttackStart;
				Print << U"ChangeAi2RunAway";
				changeAI(AIType::RunAway);
			}
		}break;

		default:
			attackState = AttackState::AttackStart;
			break;
		}
	}

	void Clown::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}

	void Clown::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void Clown::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
	}
}
