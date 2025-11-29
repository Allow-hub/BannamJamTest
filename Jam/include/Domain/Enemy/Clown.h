#pragma once
#include "Domain/Enemy/EnemyBase.h"
#include "Domain/Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	// ピエロ
	// プレイヤーを見つけると近づき、一定距離まで接近すると爆弾を投げつける
	// その後逃げる
	class Clown : public EnemyBase
	{
	public:
		explicit Clown(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
		, Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~Clown() = default;

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
		Vec2 m_offset = Vec2{ 100, 0 };
		Vec2 m_throwForce = Vec2{ 150, -300.0 };	
		double m_explosionDelay = 2.0;
	};
}
