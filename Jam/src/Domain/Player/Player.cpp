#include "Player.h"
#include "../Physics/IPhysicsBody.h"

namespace Jam::Domain::Player
{
	Player::Player(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Events::GameEventQueue& eventQueue)
		: m_body(std::move(body)), m_eventQueue(eventQueue)
	{
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::Player);
	}

	void Player::update(double deltaTime)
	{
		updateState();
	}

	void Player::moveLeft()
	{
		m_body->applyForce({ -m_stats.moveSpeed, 0 });
		m_facingRight = false;
	}

	void Player::moveRight()
	{
		m_body->applyForce({ m_stats.moveSpeed, 0 });
		m_facingRight = true;
	}

	void Player::jump()
	{
		m_eventQueue.push(Events::PlayerAttackedEvent{
					{0,0},
					true,
					50.0
				});
		if (m_isGrounded)
		{
			 m_body->applyImpulse({ 0, -m_stats.jumpPower });
			m_isGrounded = false;
		}
	}

	s3d::Vec2 Player::getPosition() const
	{
		 return m_body->getPosition();
	}

	bool Player::isFacingRight() const
	{
		return m_facingRight;
	}

	void Player::updateState()
	{
	}

	void Player::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		switch (other->getLayer())
		{
		case Jam::Domain::Physics::PhysicsLayer::Ground:
			auto v = m_body->getVelocity();
			m_body->setVelocity({ v.x, 0.0 });
			m_isGrounded = true;
			break;
		case Jam::Domain::Physics::PhysicsLayer::Enemy:
			break;
		default:
			Print(U"Not match tag");
			break;
		}
	}

	void Player::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void Player::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
}
