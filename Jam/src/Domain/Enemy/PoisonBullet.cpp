#include "Domain/Enemy/PoisonBullet.h"
#include "Infrastructure/FactoryServiceLocator.h"
#include "Infrastructure/IPhysicsBodyFactory.h"
#include "Infrastructure/PhysicsFilterManager.h"

using namespace Jam::Domain::Physics;
using namespace Jam::Infrastructure::Locator;

namespace Jam::Domain::Enemy
{
	PoisonBullet::PoisonBullet(
		std::shared_ptr<IPhysicsBody> body,
		PhysicsBodyID playerId,
		Jam::Domain::Events::GameEventQueue& queue,
		double damage,
		double lifetime,
		double homingTime,
		Vec2 size,
		Vec2 Velocity
	)
		: ToxicityBulletBase(body, playerId, queue, damage, lifetime, homingTime, size, Velocity)
	{
		setupStatusAilment();
		fbTex = Texture(Resource(U"Assets/Item/PoisonBullet_Poison.png"));
	}

	void PoisonBullet::setupStatusAilment()
	{
		m_statusType = Jam::Domain::Player::StatusAilmentType::Poison;
		m_statusDuration = 30.0;
		m_statusPower = 10.0;
		m_statusTickInterval = 3.0;
	}
}
