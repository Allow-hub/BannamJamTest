#pragma once
#include "../Physics/ICollisionListener.h"
#include "../Physics/PhysicsTypes.h"
#include "../Physics/PhysicsBodyID.h"
#include <functional>
#include "EnemyAI/IEnemyAI.h"
#include "../ITakeDamageable.h"
#include "../../Foundation/CoroutineUtil.h"
#include "../Events/GameEvents.h"

namespace Jam::Infrastructure {
	class FactoryServiceLocator;
}
namespace Jam::Domain::Enemy
{
	// エネミーのステータス情報
	struct EnemyStatus
	{
		int hp;
		double attackPower;
		double moveSpeed;
		double jumpPower;
		SizeF colSize;
		Jam::Domain::Physics::PhysicsMaterial physicsMaterial;
	};

	class EnemyBase : public Jam::Domain::Physics::ICollisionListener ,public Jam::Domain::ITakeDamageable
	{
	public:
		using AnimationEvent = std::function<void(const s3d::String&)>;

		// =========================
		// コンストラクタ / デストラクタ
		// =========================
		explicit EnemyBase(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
						   Jam::Domain::Physics::PhysicsBodyID playerId, Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~EnemyBase() = default;

		// =========================
		// 基本操作
		// =========================
		virtual void update(double deltaTime) = 0;
		virtual void moveLeft();
		virtual void moveRight();
		virtual void jump();

		bool isAlive() const override { return m_isAlive; }
		void takeDamage(const DamageInfo& info)override;
		double getCurrentHp() const override { return m_status.hp; }
		// =========================
		// 位置・物理操作
		// =========================
		Vec2 getPosition() const;
		void setPos(Vec2 p);
		void setGravityScale(double s);
		Vec2 getPlayerPos();
		// =========================
		// ステータス
		// =========================
		void setStatus(const EnemyStatus& status) { m_status = status; }
		const EnemyStatus& getStatus() const { return m_status; }

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> getPhysicsBody() { return m_body; }

		// =========================
		// 衝突イベント
		// =========================
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		// =========================
		// アニメーション
		// =========================
		void setOnAnimationChange(AnimationEvent callback) { m_onAnimChange = std::move(callback); }

		// =========================
		// AI 関連
		// =========================

		// 巡回ルート設定
		void setPatrolRoute(const Jam::Domain::Enemy::PatrolRoute& route) { m_patrolRoute = route; }
		Jam::Domain::Enemy::PatrolRoute& getPatrolRoute() { return m_patrolRoute; }
		const Jam::Domain::Enemy::PatrolRoute& getPatrolRoute() const { return m_patrolRoute; }

		virtual void onAIEvent(EnemyAIEvent e) {}; // AIからの通知を受け取るフック
		void setAIList(std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList);
		void changeAI(AIType type);
		AIType getAIType() const;

		// AI 用フック (各 AI が切り替わったときに派生クラスで処理可能)
		virtual void onPatrolEnter() {}
		virtual void onPatrolUpdate(double deltaTime) {}
		virtual void onPatrolExit() {}

		virtual void onChaseEnter() {}
		virtual void onChaseUpdate(double deltaTime) {}
		virtual void onChaseExit() {}

		virtual void onAttackEnter() {}
		virtual void onAttackUpdate(double deltaTime) {}
		virtual void onAttackExit() {}

	protected:
		// アニメーション変更通知
		void changeAnimation(const s3d::String& animName)
		{
			if (m_onAnimChange)
				m_onAnimChange(animName);
		}

		virtual void onDamaged(const DamageInfo& info) {}
		virtual Jam::Util::Task onDeath(const DamageInfo& info);
		virtual void onDestroy(const DamageInfo& info);
		bool m_isGrounded;
		// =========================
		// メンバ変数
		// =========================
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> m_aiList; // AIリスト
		IEnemyAI* m_currentAI = nullptr;                                     // 現在の AI
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;          // 物理ボディ
		EnemyStatus m_status;                                                // ステータス
		bool m_isAlive = true;                                               // 生存フラグ
		bool m_isDeadAttack = false;
		Jam::Domain::Physics::PhysicsBodyID m_playerId;                      // 追跡対象プレイヤーのID
		AnimationEvent m_onAnimChange;                                       // アニメーション変更通知
		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		Vec2 m_enemyImpluseDir = {0,0};
		EnemyType m_enemyType = EnemyType::LittleDevil;
		Jam::Domain::Enemy::PatrolRoute m_patrolRoute;
	};
}
