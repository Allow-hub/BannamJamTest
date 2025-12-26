#pragma once
#include "Domain/Enemy/ToxicityPlantBase.h"

namespace Jam::Domain::Enemy
{
	//しびれの弾を撃つ植物
	// ToxicityPlantBase を継承し、麻痺弾（ParalysisBullet）を発射する
	class ParalysisPlant : public ToxicityPlantBase
	{
	public:
		explicit ParalysisPlant(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~ParalysisPlant() = default;

	protected:
		void shootBullet(const Vec2& direction) override;
	};
}
