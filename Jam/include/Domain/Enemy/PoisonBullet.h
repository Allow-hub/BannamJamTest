#pragma once

#include "Domain/Enemy/ToxicityBulletBase.h"

namespace Jam::Domain::Enemy
{
	// 毒弾の実装
	class PoisonBullet : public ToxicityBulletBase
	{
	protected:
		// 状態異常のパラメータを設定する
		void setupStatusAilment() override;

	public:
		PoisonBullet(
			std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& queue,
			double damage,
			double lifetime,
			double homingTime,
			Vec2 size,
			Vec2 Velocity
		);

		virtual ~PoisonBullet() = default;
	};
}
