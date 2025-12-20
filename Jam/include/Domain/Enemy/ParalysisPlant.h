#pragma once
#include "Domain/Enemy/ToxicPlant.h"

namespace Jam::Domain::Enemy
{
	//しびれの弾を撃つ植物
	// ToxicPlant を継承し、麻痺弾（ParalysisBullet）を発射する
	class ParalysisPlant : public ToxicPlant
	{
	public:
		explicit ParalysisPlant(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& eventQueue)
			: ToxicPlant(body, playerId, eventQueue)
		{
			m_enemyType = Jam::Domain::EnemyType::ParalysisPlant;
		}

		virtual ~ParalysisPlant() = default;

	protected:
		void shootPoisonBullet(const Vec2& direction) override;
	};
}
