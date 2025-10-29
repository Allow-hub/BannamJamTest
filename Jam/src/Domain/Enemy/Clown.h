#pragma once
#include "EnemyBase.h"
#include "../Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	// ピエロ
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
	};
}
