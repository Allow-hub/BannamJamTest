#pragma once
#include "Domain/Enemy/EnemyBase.h"
#include "Domain/Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	// 植物系エネミーの共通基底クラス
	// 固定砲台のようにその場から弾を撃つ挙動を共通化
	class PlantBase : public EnemyBase
	{
	public:
		explicit PlantBase(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& eventQueue);

		virtual ~PlantBase() = default;

		// 毎フレームの更新（AI 挙動など）
		void update(double deltaTime) override;
		void draw() const override {};

		// 当たり判定イベント
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		void onAIEvent(EnemyAIEvent e) override;

	protected:
		void onAttackEnter() override;
		void onAttackUpdate(double deltaTime) override;
		void onAttackExit() override;

		// 弾生成処理。派生クラスで実際の弾種（毒 / 麻痺）を生成する
		virtual void shootBullet(const Vec2& direction) = 0;

		// 弾関連定数や状態
		const int attackCooldown = 500;
		const int shotInterval = 60;
		const int maxShotCount = 1;

		const float shotBulletDistance = 50.0f;
		const Vec2 size = { 40,40 };
		const double speed = 150.0;

		int elapsedTime = 0;
		int shotCount = 0;

		enum class AttackState
		{
			IsAttackStart = 0,
			IsBulletLaunch,
			IsAttackEnd,
		};
		AttackState attackState = AttackState::IsAttackStart;
	};

	// 毒の植物
	// PlantBase を継承し、毒弾（PoisonBullet）を発射する
	class PoisonPlant : public PlantBase
	{
	public:
		explicit PoisonPlant(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~PoisonPlant() = default;

	protected:
		void shootBullet(const Vec2& direction) override;
	};
}
