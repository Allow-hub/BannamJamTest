#pragma once
#include "EnemyBase.h"
#include "../Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	// 爆弾の敵クラス
	class ClownBomb : public EnemyBase
	{
	public:
		explicit ClownBomb(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
		, Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~ClownBomb() = default;

		// 毎フレームの更新
		void update(double deltaTime) override;
		void draw() const override {};

		// 当たり判定イベント（必要に応じて上書き）
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

	private:
		//タイマー関連
		const Duration exprosiveTime = SecondsF(1.0);
		Timer exprosiveTimer{ exprosiveTime,StartImmediately::No };

		//攻撃をステートパターンで管理するためのenum
		enum class AttackState
		{
			IsStart = 0,	// 飛翔開始
			IsFlyig,		// 飛翔中
			IsExplosion,	// 着弾
			EndExProsion	// 爆発終了
		};
		AttackState attackState;
	};
}
