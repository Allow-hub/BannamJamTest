#include "Domain/Player/Choker/ChokerSkill.h"
#include "Infrastructure/Siv3DCursorUtil.h"
#include "Infrastructure/FactoryServiceLocator.h"
#include "Infrastructure/IPhysicsBodyFactory.h"
#include "Presentation/AudioService.h"
#include "Infrastructure/PhysicsFilterManager.h"
#include "Domain/Player/Player.h"

#include <Siv3D.hpp>
#include "Domain/Player/Choker/ChokerStates/ChokerIdleState.h"
#include "Domain/Player/Choker/ChokerStates/ChokerFlyingState.h"

using namespace Jam::Domain::Player;
using namespace Jam::Domain::Physics;

namespace Jam::Domain::Player
{
	ChokerSkill::ChokerSkill(Jam::Domain::Events::GameEventQueue& eventQueue,
							 PhysicsBodyID ownerId,
							 Jam::Domain::Player::PlayerStats& stats,
							 Player& player)
		: m_eventQueue(eventQueue)
		, m_ownerId(ownerId)
		, m_playerStats(stats)
		, m_player(player)
	{
		m_body = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
			.getPhysicsFactory()
			->createBody(
				Vec2{ -10000, -10000 },
				SizeF{ 20, 20 },
				s3d::P2BodyType::Dynamic,
				Jam::Domain::Physics::PhysicsMaterial{ 0.0, 0.0, 0.1 },
				Jam::Domain::Physics::PhysicsShape::Circle
			);

		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::PlayerWeapon);
		m_body->setGravityScale(0);
		m_body->setBullet(true);
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::Weapon);
	}

	void ChokerSkill::init()
	{
		m_ctx = std::make_unique<ChokerContext>(
		ChokerContext{
			m_eventQueue,
			m_ownerId,
			m_playerStats,
			m_player,
			m_body,
			std::nullopt, // joint
			nullptr,      // ground
			nullptr,      // targetEnemy
			false, false, false, // isActive, isFlying, isHooked
			0.0, 0.0,			 // flyTimer, cooldownTimer
			Vec2{0,0}, Vec2{0,0},// lastDir, groundAnchorOffset
			*this
		});

		m_body->setCollisionListener(shared_from_this());
		transitionTo<ChokerIdleState>();
	}

	void ChokerSkill::releaseJoint()
	{
		if (m_joint.has_value())
		{
			m_joint->release();
			m_joint.reset();
		}
	}

	void ChokerSkill::resetHook()
	{
		m_body->setVelocity({ 0, 0 });
		m_body->setAngularVelocity(0);
		m_body->setBodyType(PhysicsType::Dynamic);
		m_body->setGravityScale(0);
	}

	void ChokerSkill::use(const Vec2 position, bool facingRight)
	{
		if (m_ctx && m_ctx->cooldownTimer > 0.0)
			return;
		transitionTo<ChokerFlyingState>();
		// クールダウン開始
		m_ctx->cooldownTimer = 0.13;
	}

	void ChokerSkill::useReleased(const Vec2 position, bool facingRight)
	{
		auto* hooked = dynamic_cast<HookedEnemyState*>(m_state.get());
		auto* idle = dynamic_cast<ChokerIdleState*>(m_state.get());

		if (idle || hooked)
			return; // HookedEnemy のときは弾く
		transitionTo<ChokerIdleState>();
	}

	void ChokerSkill::update(double deltaTime)
	{
		if (m_state)
			m_state->update(*m_ctx, deltaTime);

		if (m_ctx->cooldownTimer > 0.0)
			m_ctx->cooldownTimer = std::max(0.0, m_ctx->cooldownTimer - deltaTime);
	}

	void ChokerSkill::draw() const
	{
		if (m_state)
			m_state->draw(*m_ctx);
	}

	bool ChokerSkill::needUpdate() const
	{
		return m_ctx->isActive || m_ctx->cooldownTimer > 0.0/* || m_ctx->isInEnemyHitFreeze*/;
	}

	void ChokerSkill::onCollisionEnter(std::shared_ptr<IPhysicsBody> other)
	{
		if (m_state)
			m_state->onCollisionEnter(*m_ctx, { other });
	}

	void ChokerSkill::onCollisionStay(std::shared_ptr<IPhysicsBody> other) {}
	void ChokerSkill::onCollisionExit(std::shared_ptr<IPhysicsBody> other) {}

	void ChokerSkill::onDeactivate()
	{
		if (m_joint.has_value())
		{
			resetHook();
			releaseJoint();
		}
	}

	ChokerSkill::~ChokerSkill()
	{
		releaseJoint();
	}
}
