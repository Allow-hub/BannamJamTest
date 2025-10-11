#pragma once
//#include "../Domain/Player.hpp"
//#include "../Infrastructure/PhysicsWrapper.hpp"

namespace Jam::UseCase
{
	class PlayerService
	{
	//public:
	//	PlayerService(Domain::Player& player, PhysicsWrapper& physics)
	//		: m_player(player), m_physics(physics) {
	//	}

	//	void update(const Game::InputManager& input)
	//	{
	//		handleMovement(input);
	//		handleJump(input);
	//		applyPhysics();
	//	}

	//private:
	//	void handleMovement(const Game::InputManager& input)
	//	{
	//		if (input.left())
	//			m_player.velocity.x = -m_player.speed;
	//		else if (input.right())
	//			m_player.velocity.x = m_player.speed;
	//		else
	//			m_player.velocity.x = 0;
	//	}

	//	void handleJump(const Game::InputManager& input)
	//	{
	//		if (input.jump() && m_player.isGrounded)
	//		{
	//			m_player.velocity.y = -m_player.jumpPower;
	//			m_player.isGrounded = false;
	//		}
	//	}

	//	void applyPhysics()
	//	{
	//		m_physics.applyGravity(m_player);
	//		m_physics.moveAndCollide(m_player);
	//	}

	//	Domain::Player& m_player;
	//	PhysicsWrapper& m_physics;
	};
}
