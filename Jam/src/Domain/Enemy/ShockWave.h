#pragma once
#include <memory>
#include "../Physics/IPhysicsBody.h"
#include "../Events/GameEvents.h"
#include "../Physics/ICollisionListener.h"

namespace Jam::Domain::Enemy
{
	class ShockWave : public Jam::Domain::Physics::ICollisionListener
					, public std::enable_shared_from_this<ShockWave>
	{
	private:
		std::vector<s3d::Texture> m_textures;
		int m_frameIndex = 0;
		double m_animTimer = 0.0;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		double m_attackPower = 10.0;
		double m_timer = 0.0;
		double m_duration;
		Vec2 m_startRect;
		Vec2 m_endRect;
		bool m_finished = false;
		bool m_hittedPlayer = false;//一回のみ
		double m_lastRegenTime = 0.0;
		Jam::Domain::Physics::PhysicsBodyID m_playerId;

	public:
		ShockWave(const Vec2& pos, double duration, Vec2 startRect, Vec2 endRect, Jam::Domain::Physics::PhysicsBodyID playerId, Jam::Domain::Events::GameEventQueue& queue);


		void update(double dt);
		void draw() const;

		bool isFinished() const { return m_finished; }

		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
	};
}
