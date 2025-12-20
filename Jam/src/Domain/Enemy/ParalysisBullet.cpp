#include "Domain/Enemy/ParalysisBullet.h"

namespace Jam::Domain::Enemy
{
	ParalysisBullet::ParalysisBullet(
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
		Jam::Domain::Physics::PhysicsBodyID playerId,
		Jam::Domain::Events::GameEventQueue& queue,
		double damage,
		double lifetime,
		double homingTime,
		Vec2 size,
		Vec2 Velocity
	)
		: PoisonBullet(body, playerId, queue, damage, lifetime, homingTime, size, Velocity)
	{
		setupStatusAilment();
		fbTex = Texture(Resource(U"Assets/Item/PoisonBullet_Paralysis.png"));
	}

	void ParalysisBullet::setupStatusAilment()
	{
		m_statusType = Jam::Domain::Player::StatusAilmentType::Paralysis;
		m_statusDuration = 7.0;	// 麻痺の持続時間
		m_statusPower = 0.3;	// 麻痺の効果(移動・ジャンプ速度低下率)
	}
}
