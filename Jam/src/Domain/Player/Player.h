#pragma once
#include <memory>
#include "../Physics/IPhysicsBody.h"
#include "../Physics/ICollisionListener.h"
#include "../Events/GameEvents.h"
#include "Skill/IPlayerSkill.h"

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
	class Player :public Jam::Domain::Physics::ICollisionListener
	{
	public:
		explicit Player(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Events::GameEventQueue& eventQueue);

		void update(double deltaTime);
		void draw()const;

		void moveLeft();
		void moveRight();
		void startDash();
		void endDash();
		void attack();

		void skillPush();
		void skillReleased();
		void changeSkill(int direction);
		void jump();

		s3d::Vec2 getPosition() const;
		bool isFacingRight() const;

		void setSpeed(double s) { m_stats.moveSpeed = s; }
		void setJumpPower(double j) { m_stats.jumpPower = j; }
		std::shared_ptr<Domain::Physics::IPhysicsBody> getPhysicsBody() { return m_body; }
		double getHookedSpeedMultiplier() const;
		// ICollisionListener
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

	private:
		PlayerStats m_stats;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		bool m_isGrounded = false;
		bool m_facingRight = true;
		bool m_isDashing = false;
		double dashMagnification = 2.0;
		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		std::vector<std::shared_ptr<IPlayerSkill>> m_skills;
		std::shared_ptr<IPlayerSkill> m_currentSkill;

		void updateState();
	};
}
