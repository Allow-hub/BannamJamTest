#pragma once
#include <memory>
#include "../Physics/IPhysicsBody.h"

namespace Jam::Domain::Player
{
	struct PlayerStats
	{
		double moveSpeed;
		double jumpPower;
		Jam::Domain::Physics::PhysicsMaterial physicsMaterial;
	};

	// プレイヤーキャラクターを表すクラス
	// 他クラスに依存しない
	class Player
	{
	public:
		explicit Player(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body);

		void update(double deltaTime);

		void moveLeft();
		void moveRight();
		void jump();
		void onGroundContact(bool grounded);

		s3d::Vec2 getPosition() const;
		bool isFacingRight() const;

		void setSpeed(double s) { m_stats.moveSpeed = s; }
		void setJumpPower(double j) { m_stats.jumpPower = j; }
		std::shared_ptr<Domain::Physics::IPhysicsBody> getPhysicsBody() { return m_body; }
	private:
		PlayerStats m_stats;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		bool m_isGrounded = false;
		bool m_facingRight = true;

		void updateState();
	};
}
