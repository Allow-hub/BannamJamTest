	#pragma once
#include "EnemyBase.h"
#include "../Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	// リボンタイプの敵クラス
	class Ribbon : public EnemyBase
	{
	public:
		explicit Ribbon(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
		, Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~Ribbon() = default;

		// 毎フレームの更新（AI挙動など）
		void update(double deltaTime) override;

		// 当たり判定イベント（必要に応じて上書き）
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		void onAIEvent(EnemyAIEvent e) override;

	private:
		//プレイヤーが右にいるかどうか
		bool IsRight;

		//タイマー関連
		const Duration AttackInterval = SecondsF(2.0);
		Timer attackTimer{ AttackInterval,StartImmediately::No };

		enum class AttackState
		{
			AttackStart = 0,
			WaitAttack,
			IsAttack,
			EndAttack
		};
		AttackState attackState;

	protected:
		void onPatrolEnter() override;
		void onPatrolUpdate(double deltaTime) override;
		void onChaseEnter() override;
		void onChaseUpdate(double deltaTime) override;
		void onAttackEnter()override;
		void onAttackUpdate(double deltaTime) override;
	};
}
