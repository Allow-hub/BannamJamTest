#pragma once
#include "Domain/Enemy/EnemyBase.h"
#include "Domain/Events/GameEvents.h"

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
		const int attackCooldown = 420;	// 攻撃後のクールタイム
		const int shotInterval = 60;	// 鬼火を撃つ間隔
		const int maxShotCount = 3;		// 鬼火を撃つ回数

		const float shotFireBallDistance = 50.0f;	// 鬼火を撃つ位置の距離
		const Vec2 size = { 40, 40 };	// 鬼火のサイズ
		const double speed = 400.0;		// 鬼火の速度

		int elapsedTime = 0;	// 経過時間
		int shotCount = 0;		// 鬼火を撃った回数

		enum class AttackState
		{
			IsAttackStart = 0,
			IsFireBallLaunch,
			IsAttackEnd,
		};
		AttackState attackState = AttackState::IsAttackStart;

		void shootFireball(const Vec2& direction);
	};
}
