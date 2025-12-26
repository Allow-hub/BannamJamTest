#pragma once
#include "Domain/Enemy/ToxicityBulletBase.h"

namespace Jam::Domain::Enemy
{
	class ParalysisBullet : public ToxicityBulletBase
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
		void setupStatusAilment() override;
	};
}
