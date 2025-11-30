#pragma once
#include "Domain/Player/Player.h"
#include "Domain/Physics/IPhysicsBody.h"
#include "Domain/Events/GameEvents.h"

namespace Jam::Domain::Player
{
	class Player;
	class ChokerSkill;
	class PlayerStats;

	struct ChokerContext
	{
		Jam::Domain::Events::GameEventQueue& eventQueue;
		Jam::Domain::Physics::PhysicsBodyID ownerId;
		Jam::Domain::Player::PlayerStats& stats;
		Player& player;

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body;
		std::optional<P2DistanceJoint> joint;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> ground;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> targetEnemy;

		bool isActive = false;
		bool isFlying = false;
		bool isHooked = false;

		double flyTimer = 0.0;
		double cooldownTimer = 0.0;

		Vec2 lastDir;
		Vec2 groundAnchorOffset;

		ChokerSkill& skill;
	};
}
