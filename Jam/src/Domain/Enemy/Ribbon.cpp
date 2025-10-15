#include "Ribbon.h"

namespace Jam::Domain::Enemy
{
	Ribbon::Ribbon(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, std::shared_ptr<Jam::Domain::Player::Player>& player)
		: EnemyBase(body,player)
	{
	}

	void Ribbon::update(double deltaTime)
	{
		Print << m_player->getPosition();
		if (!isAlive()) return;

		m_patrolTimer += deltaTime;

		switch (m_state)
		{
		case State::Idle:
		{
			if (m_patrolTimer >= 2.0f)
			{
				m_patrolTimer = 0;
				m_state = State::Patrol;
			}
		}
		break;
		case State::Patrol:
		{
			//往復移動
			switch (ActionPt_i)
			{
			case 0:
			{
				if (m_patrolTimer >= 5.0f)
				{
					m_patrolTimer = 0;
					ActionPt_i = 1;
					m_state = State::Idle;
				}
				else
				{
					moveLeft();
				}
			}
			break;
			case 1:
			{
				if (m_patrolTimer >= 5.0f)
				{
					m_patrolTimer = 0;
					ActionPt_i = 0;
					m_state = State::Idle;
				}
				else
				{
					moveRight();
				}
			}
			break;
			}
		}
		break;
		case State::Discover:
		{
			m_state = State::Idle;
		}
		break;
		case State::GetClose:
		{
			m_state = State::Idle;
		}
		break;
		case State::Attack:
		{

			m_state = State::Idle;
		}
		break;
		}

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

	void Ribbon::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// TODO: 当たり判定で攻撃処理などを実装
		Print(U"HIT");
	}

	void Ribbon::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// 現状は何もしない
	}

	void Ribbon::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// 現状は何もしない
	}
}

