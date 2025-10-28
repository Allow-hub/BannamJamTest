#include "Boss1_3.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"
#include "EnemyAI/AttackAI.h"
#include <random>

namespace Jam::Domain::Enemy
{
	Boss1_3::Boss1_3(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
					 Jam::Domain::Physics::PhysicsBodyID playerId,
					 Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
		, currentBossState(BossState::Appear)
		, currentAttackState(AttackState::Missile)
		, m_stateTimer(0.0)
		, m_weakStateDuration(5.0)
		, m_appearDuration(3.0)
		, m_isReflectedMissileHit(false)
		, m_attackCooldownTimer(0.0)
		, m_attackCooldown(2.0)
		, m_isAttacking(false)
		, m_hasAttackEntered(false)
		, m_missileAttackDuration(3.0)
		, m_summonClownDuration(2.0)
		, m_bombAttackDuration(1.5)
	{
		m_enemyType = EnemyType::Boss1_3;
		m_body->setGravityScale(0);

		// 攻撃パターンの確率設定
		m_attackPatterns = {
			{AttackState::Missile, 0.5f},
			{AttackState::SummonClown, 0.3f},
			{AttackState::Bomb, 0.2f}
		};
	}

	void Boss1_3::update(double deltaTime)
	{
		if (!isAlive()) return;

		m_stateTimer += deltaTime;

		switch (currentBossState)
		{
		case BossState::Appear:
			updateAppearState(deltaTime);
			break;
		case BossState::Normal:
			updateNormalState(deltaTime);
			break;
		case BossState::Weak:
			updateWeakState(deltaTime);
			break;
		case BossState::Dead:
			// 死亡演出処理
			break;
		}
	}

	void Boss1_3::updateAppearState(double deltaTime)
	{
		// 登場演出
		if (m_stateTimer >= m_appearDuration)
		{
			currentBossState = BossState::Normal;
			m_stateTimer = 0.0;
			m_attackCooldownTimer = 0.0;
		}
	}

	void Boss1_3::updateNormalState(double deltaTime)
	{
		// 反射ミサイルが当たったかチェック
		if (m_isReflectedMissileHit)
		{
			// 現在の攻撃を終了
			if (m_isAttacking && m_hasAttackEntered)
			{
				switch (currentAttackState)
				{
				case AttackState::Missile:
					exitMissileAttack();
					break;
				case AttackState::SummonClown:
					exitSummonClown();
					break;
				case AttackState::Bomb:
					exitBombAttack();
					break;
				}
			}

			currentBossState = BossState::Weak;
			m_stateTimer = 0.0;
			m_isReflectedMissileHit = false;
			m_isAttacking = false;
			m_hasAttackEntered = false;
			m_attackCooldownTimer = 0.0;
			return;
		}

		// 攻撃中の処理
		if (m_isAttacking)
		{
			m_attackCooldownTimer += deltaTime;

			// 攻撃パターンの実行
			switch (currentAttackState)
			{
			case AttackState::Missile:
				if (!m_hasAttackEntered)
				{
					enterMissileAttack();
					m_hasAttackEntered = true;
				}
				updateMissileAttack(deltaTime);

				if (m_attackCooldownTimer >= m_missileAttackDuration)
				{
					exitMissileAttack();
					m_isAttacking = false;
					m_hasAttackEntered = false;
					m_attackCooldownTimer = 0.0;
				}
				break;

			case AttackState::SummonClown:
				if (!m_hasAttackEntered)
				{
					enterSummonClown();
					m_hasAttackEntered = true;
				}
				updateSummonClown(deltaTime);

				if (m_attackCooldownTimer >= m_summonClownDuration)
				{
					exitSummonClown();
					m_isAttacking = false;
					m_hasAttackEntered = false;
					m_attackCooldownTimer = 0.0;
				}
				break;

			case AttackState::Bomb:
				if (!m_hasAttackEntered)
				{
					enterBombAttack();
					m_hasAttackEntered = true;
				}
				updateBombAttack(deltaTime);

				if (m_attackCooldownTimer >= m_bombAttackDuration)
				{
					exitBombAttack();
					m_isAttacking = false;
					m_hasAttackEntered = false;
					m_attackCooldownTimer = 0.0;
				}
				break;
			}
		}
		else
		{
			// クールダウン中
			m_attackCooldownTimer += deltaTime;

			if (m_attackCooldownTimer >= m_attackCooldown)
			{
				// 次の攻撃を抽選で選択
				currentAttackState = selectNextAttack();
				m_isAttacking = true;
				m_attackCooldownTimer = 0.0;
			}
		}
	}

	void Boss1_3::updateWeakState(double deltaTime)
	{
		// 弱点露出中は何もしない
		if (m_stateTimer >= m_weakStateDuration)
		{
			currentBossState = BossState::Normal;
			m_stateTimer = 0.0;
			m_attackCooldownTimer = 0.0;
			m_isAttacking = false;
			m_hasAttackEntered = false;
		}
	}

	Boss1_3::AttackState Boss1_3::selectNextAttack()
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);

		float roll = dis(gen);
		float cumulative = 0.0f;

		for (const auto& pattern : m_attackPatterns)
		{
			cumulative += pattern.probability;
			if (roll <= cumulative)
			{
				return pattern.state;
			}
		}

		return Boss1_3::AttackState::Missile;
	}

	// ===== Missile Attack =====
	void Boss1_3::enterMissileAttack()
	{
		Print << U"Enter: Missile Attack";
		// TODO: 初期化処理
	}

	void Boss1_3::updateMissileAttack(double deltaTime)
	{
		// TODO: ミサイル発射処理
	}

	void Boss1_3::exitMissileAttack()
	{
		Print << U"Exit: Missile Attack";
		// TODO: 終了処理
	}

	// ===== Summon Clown =====
	void Boss1_3::enterSummonClown()
	{
		Print << U"Enter: Summon Clown";
		// TODO: 召喚開始処理
	}

	void Boss1_3::updateSummonClown(double deltaTime)
	{
		// TODO: 召喚中の処理
	}

	void Boss1_3::exitSummonClown()
	{
		Print << U"Exit: Summon Clown";
		// TODO: 召喚終了処理
	}

	// ===== Bomb Attack =====
	void Boss1_3::enterBombAttack()
	{
		Print << U"Enter: Bomb Attack";
		// TODO: 爆弾投擲開始処理
	}

	void Boss1_3::updateBombAttack(double deltaTime)
	{
		// TODO: 爆弾投擲処理
	}

	void Boss1_3::exitBombAttack()
	{
		Print << U"Exit: Bomb Attack";
		// TODO: 投擲終了処理
	}

	void Boss1_3::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);

		// TODO: 反射されたミサイルとの衝突判定
		// if (other->getType() == )
		// {
		//     m_isReflectedMissileHit = true;
		// }
	}

	void Boss1_3::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
	}

	void Boss1_3::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionExit(other);
	}
}
