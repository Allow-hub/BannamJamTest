#pragma once
#include "Domain/Enemy/PoisonPlant.h"

namespace Jam::Domain::Enemy
{
	//しびれの弾を撃つ植物
	// PoisonPlant を継承し、麻痺弾（ParalysisBullet）を発射する
	class ParalysisPlant : public PoisonPlant
	{
	public:
		explicit ParalysisPlant(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& eventQueue)
			: PoisonPlant(body, playerId, eventQueue)
		{
			m_enemyType = Jam::Domain::EnemyType::ParalysisPlant;
		}

		virtual ~ParalysisPlant() = default;

	protected:
		void shootBullet(const Vec2& direction) override;
	};
}
