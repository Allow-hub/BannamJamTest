#pragma once
#include "EnemyBase.h"
#include "../Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	// 小悪魔タイプの敵クラス
	// 飛行や短距離突進など、特徴的な挙動を今後追加予定
	class Clown : public EnemyBase
	{
	public:
		explicit Clown(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
		, Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~Clown() = default;

		// 毎フレームの更新（AI挙動など）
		void update(double deltaTime) override;

		// 当たり判定イベント（必要に応じて上書き）
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		void onAIEvent(EnemyAIEvent e) override;

	private:
		bool IsRight;

		//タイマー関連
		const Duration AttackInterval = SecondsF(1.0);
		Timer attackTimer{ AttackInterval,StartImmediately::No };

		//攻撃をステートパターンで管理するためのenum
		enum class AttackState
		{
			AttackStart = 0,
			WaitAttack,
			IsAttack,
			EndAttack
		};
		AttackState attackState;

	protected:
		void onChaseEnter() override;
		void onChaseUpdate(double deltaTime) override;
		void onAttackEnter()override;
		void onAttackUpdate(double deltaTime) override;
	};
}
