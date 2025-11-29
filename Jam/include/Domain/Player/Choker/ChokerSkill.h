#pragma once
#include "../../Physics/ICollisionListener.h"
#include "../../Events/GameEvents.h"
#include "../../../Foundation/CoroutineUtil.h"
#include "ChokerStates/IChokerState.h"

namespace Jam::Domain::Player
{
	class Player;
	struct PlayerStats;

	// チョーカー（グラップリングフック）- Playerの固有能力
	class ChokerSkill
		: public Jam::Domain::Physics::ICollisionListener
		, public std::enable_shared_from_this<ChokerSkill>
	{
	private:
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		std::optional<s3d::P2DistanceJoint> m_joint;
		Player& m_player;
		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		Jam::Domain::Player::PlayerStats& m_playerStats;

		const Vec2 createOffset = Vec2{ 50, -30 };
		Jam::Domain::Physics::PhysicsBodyID m_ownerId;

	public:
		ChokerSkill(Jam::Domain::Events::GameEventQueue& eventQueue,
					Jam::Domain::Physics::PhysicsBodyID ownerId,
					Jam::Domain::Player::PlayerStats& stats,
					Player& player);

		void init();
		void use(const s3d::Vec2 position, bool facingRight);
		void useReleased(const s3d::Vec2 position, bool facingRight);
		void update(double deltaTime);
		void draw() const;
		bool needUpdate() const;

		// ICollisionListener
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		void onDeactivate();

		void releaseJoint();
		void resetHook();

		template<class TState, class... Args>
		void transitionTo(Args&&... args)
		{
			if (m_state)
				m_state->exit(*m_ctx);
			m_state = std::make_unique<TState>(std::forward<Args>(args)...);
			m_state->enter(*m_ctx);
		}

		std::unique_ptr<IChokerState> m_state;
		std::shared_ptr<ChokerContext> m_ctx;

		~ChokerSkill();
	};
}
