#pragma once
#include "EnemyBase.h"
#include "../Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	// ゴスロリ人形
	// 浮遊しながらプレイヤーに近づき、プレイヤーが射程距離に入ると3つの鬼火を体の周りに出し、プレイヤーに向かって連続で射出
	// 射出後は少しの間動きを止める

	class GothicLolitaDoll : public EnemyBase
	{
	public:
		explicit GothicLolitaDoll(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
		, Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~GothicLolitaDoll() = default;

		// 毎フレームの更新（AI挙動など）
		void update(double deltaTime) override;
		void draw() const override {};

		// 当たり判定イベント（必要に応じて上書き）
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		void onAIEvent(EnemyAIEvent e) override;

	protected:
		void onAttackEnter()override;
		void onAttackUpdate(double deltaTime)override;
		void onAttackExit()override;

	private:
		double m_patrolTimer = 0.0;
		int AttackWaitTime = 0;
		enum class m_AttackStatus
		{
			IsAttackStart = 0,
			IsFireBallLaunch,
			IsAttackEnd,
		};
		m_AttackStatus m_attackStatus = m_AttackStatus::IsAttackStart;
	};
}
