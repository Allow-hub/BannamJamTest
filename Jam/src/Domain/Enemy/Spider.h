#pragma once
#include "EnemyBase.h"
#include "../Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	// スパイダー
	// プレイヤーを見つけたらとびかかる
	class Spider : public EnemyBase
	{
	public:
		explicit Spider(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
		, Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~Spider() = default;

		// 毎フレームの更新（AI挙動など）
		void update(double deltaTime) override;

		// 当たり判定イベント（必要に応じて上書き）
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		void onAIEvent(EnemyAIEvent e) override;

	protected:
		void onAttackEnter() override;
		void onAttackUpdate(double deltaTime) override;
		void onAttackExit() override;

	private:
		double m_patrolTimer = 0.0;
		// 飛びかかりの撃力
		double pounceHorizonalImpulse = 3000.0;
		double pounceVerticalImpulse = 3000.0;
		bool m_attacked = false;
		// 着地してからの時間を計るタイマー
		double loadingWaitTime = 0.2;
		double m_landedTimer = 0.0; 
		bool m_isLanding = false;
		// 攻撃後のクールダウンタイマー
		double m_attackCooldownTimer = 0.0;
		// クールダウン時間（例: 1.0秒）
		static constexpr double ATTACK_COOLDOWN_TIME = 1.0;
		bool m_attackCanceled = false;
	};
}
