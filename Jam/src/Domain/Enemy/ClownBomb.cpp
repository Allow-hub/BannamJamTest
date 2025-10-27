#include "ClownBomb.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"
#include "EnemyAI/AttackAI.h"
#include "EnemyAI/RunAwayAI.h"

namespace Jam::Domain::Enemy
{
	ClownBomb::ClownBomb(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
	, Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
	{
		//m_enemyType = EnemyType::Clown; Todo:EnemyTypeに爆弾を追加
		m_body->setGravityScale(1);
	}

	void ClownBomb::update(double deltaTime)
	{
		if (!isAlive()) return;
		switch (attackState)
		{
		case Jam::Domain::Enemy::ClownBomb::AttackState::IsStart:
			//飛翔方向を計算する
			break;
		case Jam::Domain::Enemy::ClownBomb::AttackState::IsFlyig:
			//処理なし
			break;
		case Jam::Domain::Enemy::ClownBomb::AttackState::IsExplosion:
			//一定時間経過後にEndExprosionに
			break;
		case Jam::Domain::Enemy::ClownBomb::AttackState::EndExProsion:
			//削除処理(isAlive = false)
			break;
		default:
			break;
		}
	}

	void ClownBomb::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
		if (!isAlive()) return;
		switch (attackState)
		{
		case Jam::Domain::Enemy::ClownBomb::AttackState::IsStart:
			//処理なし
			break;
		case Jam::Domain::Enemy::ClownBomb::AttackState::IsFlyig:
			//enemy以外に当たると爆発状態に
			break;
		case Jam::Domain::Enemy::ClownBomb::AttackState::IsExplosion:
			//攻撃当たり判定を出す。EyeBeam参考
			break;
		case Jam::Domain::Enemy::ClownBomb::AttackState::EndExProsion:
			//処理なし
			break;
		default:
			break;
		}
	}

	void ClownBomb::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void ClownBomb::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
}
