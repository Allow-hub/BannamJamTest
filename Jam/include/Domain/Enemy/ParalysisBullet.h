#pragma once
#include "Domain/Enemy/PoisonBullet.h"

namespace Jam::Domain::Enemy
{
	// 麻痺弾
	// 基本挙動は PoisonBullet と同じで、与える状態異常とテクスチャのみ異なる
	class ParalysisBullet : public PoisonBullet
	{
	public:
		ParalysisBullet(
			std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& queue,
			double damage,
			double lifetime,
			double homingTime,
			Vec2 size,
			Vec2 Velocity
		);

	protected:
		// テクスチャパスと状態異常パラメータを差し替え
		void setupStatusAilment() override;
	};
}
