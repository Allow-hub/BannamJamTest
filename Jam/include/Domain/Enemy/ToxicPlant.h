#pragma once
#include "Domain/Enemy/EnemyBase.h"
#include "Domain/Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	// 毒の植物
	// 指定の位置から動かず、プレイヤーが射程距離に入ると毒の弾をプレイヤーに向かって射出
	// 射出後は少しの間動きを止める

	class ToxicPlant : public EnemyBase
	{
	public:
		explicit ToxicPlant(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
		, Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~ToxicPlant() = default;

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

		// 毒弾生成処理（派生クラスからも利用・上書き可能）
		virtual void shootPoisonBullet(const Vec2& direction);

		// 弾関連定数や状態
		const int attackCooldown =500; 	// 攻撃後のクールタイム
		const int shotInterval =60; 	// 弾を撃つ間隔
		const int maxShotCount =1; 		// 弾を撃つ回数

		const float shotBulletDistance =50.0f; 	// 弾を撃つ位置の距離
		const Vec2 size = {40,40 }; 	// 弾のサイズ
		const double speed =150.0; 		// 弾の速度

		int elapsedTime =0; 	// 経過時間
		int shotCount =0; 		// 弾を撃った回数

		enum class AttackState
		{
			IsAttackStart =0,
			IsBulletLaunch,
			IsAttackEnd,
		};
		AttackState attackState = AttackState::IsAttackStart;
	};
}
