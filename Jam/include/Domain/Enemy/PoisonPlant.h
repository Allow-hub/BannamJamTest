#pragma once
#include "Domain/Enemy/ToxicityPlantBase.h"
#include "Domain/Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	class PoisonPlant : public ToxicityPlantBase
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
