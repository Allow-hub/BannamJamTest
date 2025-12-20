#pragma once
#include "Domain/Enemy/ToxicPlant.h"

namespace Jam::Domain::Enemy
{
	//しびれの弾を撃つ植物
	//ToxicPlantを継承し、弾に麻痺状態異常を付与する
	class ParalysisPlant : public ToxicPlant
	{
	public:
		explicit ParalysisPlant(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& eventQueue)
			: ToxicPlant(body, playerId, eventQueue)
		{
			// 弾が付与する状態異常を "麻痺" に変更
			setBulletStatusAilment(Jam::Domain::Player::StatusAilmentType::Paralysis);
			m_enemyType = Jam::Domain::EnemyType::ParalysisPlant;
		}

		virtual ~ParalysisPlant() = default;
	};
}
