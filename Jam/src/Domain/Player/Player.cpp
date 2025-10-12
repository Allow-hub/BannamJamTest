#include "Player.h"
#include "../Physics/IPhysicsBody.h"

namespace Jam::Domain::Player
{
	Player::Player(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body)
		: m_body(std::move(body))
	{
		m_isGrounded = m_body->isGrounded();
	}

	void Player::update(double deltaTime)
	{
		m_isGrounded = m_body->isGrounded();
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
		Print(m_isGrounded);
		if (m_isGrounded)
		{
			 m_body->applyImpulse({ 0, -m_stats.jumpPower });
			m_isGrounded = false;
		}
	}

	void Player::onGroundContact(bool grounded)
	{
		m_isGrounded = grounded;
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
}
