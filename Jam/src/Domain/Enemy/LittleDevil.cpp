#include "LittleDevil.h"

namespace Jam::Domain::Enemy
{
	LittleDevil::LittleDevil(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body)
		: EnemyBase(body)
	{
	}

	void LittleDevil::update(double deltaTime)
	{
		if (!isAlive()) return;
		moveRight();
		//m_patrolTimer += deltaTime;

		//switch (m_state)
		//{
		//case State::Idle:
		//	// 簡単なパトロール移動（今後AI強化予定）
		//	if (m_patrolTimer > 1.0)
		//	{
		//		moveLeft();
		//		m_state = State::Patrol;
		//		m_patrolTimer = 0.0;
		//	}
		//	break;

		//case State::Patrol:
		//	if (m_patrolTimer > 1.0)
		//	{
		//		moveRight();
		//		m_state = State::Idle;
		//		m_patrolTimer = 0.0;
		//	}
		//	break;

		//case State::Attack:
		//	// TODO: 攻撃パターン実装予定
		//	break;

		//case State::Dead:
		//	// TODO: 死亡アニメーション・エフェクト処理
		//	break;
	//}
	}

	void LittleDevil::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// TODO: 当たり判定で攻撃処理などを実装
		//Print(U"HIT");
	}

	void LittleDevil::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// 現状は何もしない
	}

	void LittleDevil::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// 現状は何もしない
	}
}
