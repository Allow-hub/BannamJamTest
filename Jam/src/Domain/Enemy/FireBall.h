#pragma once
#include <memory>
#include "../Physics/IPhysicsBody.h"
#include "../Events/GameEvents.h"
#include "../Physics/ICollisionListener.h"
#include "../IIndependentObject.h"

namespace Jam::Domain::Enemy
{
	class Fireball : public Jam::Domain::IIndependentObject
		, public Jam::Domain::Physics::ICollisionListener
		, public std::enable_shared_from_this<Fireball>
	{
	private:
		Texture m_texture;

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		Jam::Domain::Physics::PhysicsBodyID m_playerId;

		double m_damage;
		double m_lifetime;
		double m_timer;

		// 移動用パラメータ
		Vec2 m_Velocity;
		Vec2 m_size;
		Vec2 m_scaled = Vec2{ 0.1 ,0.1 };
		bool m_isHit = false;

	public:
		Fireball(
			std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& queue,
			double damage,
			double lifetime,
			Vec2 size,
			Vec2 Velocity
		);
		~Fireball();

		void init();
		void update(double deltaTime) override;
		void draw() const override;

		// 衝突検知
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
	};
}
