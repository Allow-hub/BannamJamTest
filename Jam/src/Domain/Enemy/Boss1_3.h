#pragma once
#include "EnemyBase.h"
#include "../Events/GameEvents.h"
#include "ShockWave.h"
#include "../../Foundation/CoroutineUtil.h"

namespace Jam::Domain::Enemy
{
	// 1_3のボス
	class Boss1_3 : public EnemyBase
	{
	public:
		explicit Boss1_3(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
		, Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~Boss1_3() = default;

		// 毎フレームの更新（AI挙動など）
		void update(double deltaTime) override;
		void draw() const override;

		// 当たり判定イベント（必要に応じて上書き）
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

	private:
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_weakBody;//弱点箇所
		Vec2 coreSize = Vec2{ 100,100 };
		Vec2 m_coreOffset;

		void updateAppearState(double deltaTime);
		void updateNormalState(double deltaTime);
		void updateWeakState(double deltaTime);

		enum class BossState
		{
			Appear,//登場
			Normal,//通常
			Weak, //弱点露出
			Dead//死亡時
		};

		BossState currentBossState;

		enum class AttackState
		{
			Missile,//ミサイル発射
			SummonClown,//ピエロ召喚
			Bomb,//爆弾を投げる
			Shockwave,//ジャンプして衝撃波
		};

		AttackState currentAttackState;

		struct AttackPattern
		{
			AttackState state;
			float probability;  // 0.0f ～ 1.0f
		};

		std::vector<AttackPattern> m_attackPatterns;

		// 攻撃パターンを抽選
		AttackState selectNextAttack();

		double m_stateTimer = 0.0;           // 状態遷移用タイマー
		double m_weakStateDuration = 5.0;    // Weak状態の持続時間
		double m_appearDuration = 3.0;       // Appear状態の持続時間
		bool m_isReflectedMissileHit = false;  // 反射ミサイルが当たったか

		// 攻撃クールダウン用
		double m_attackCooldownTimer;      // 攻撃クールダウンタイマー
		double m_attackCooldown;           // 攻撃間隔（秒）
		bool m_isAttacking;                // 現在攻撃中か

		// 各攻撃の実行時間
		double m_missileAttackDuration;
		double m_summonClownDuration;
		double m_bombAttackDuration;
		double m_shockWaveDuration;
		double m_shockJumpForce = 400000.0;

		// ピエロ召喚の設定
		static constexpr int CLOWN_SPAWN_COUNT = 2;  // 1回あたりの召喚数

		bool m_hasAttackEntered;  // 攻撃のEnterが実行されたか

		// 攻撃のライフサイクルメソッド
		// Missile
		void enterMissileAttack();
		void updateMissileAttack(double deltaTime);
		void exitMissileAttack();

		// SummonClown
		void enterSummonClown();
		void updateSummonClown(double deltaTime);
		void exitSummonClown();

		// Bomb
		void enterBombAttack();
		void updateBombAttack(double deltaTime);
		void exitBombAttack();

		// shockWave
		void enterShockWave();
		void updateShockWave(double deltaTime);
		void exitShockWave();
		std::shared_ptr<ShockWave> m_shockWave;
		const double m_shockWaveDelay = 1.2;
		Jam::Util::Task m_shockWaveTask();

		const String toString(Boss1_3::AttackState state)
		{
			switch (state)
			{
			case Boss1_3::AttackState::Missile: return U"Missile";
			case Boss1_3::AttackState::SummonClown: return U"SummonClown";
			case Boss1_3::AttackState::Bomb: return U"Bomb";
			default: return U"Unknown";
			}
		}
	};
}
