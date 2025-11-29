#pragma once
#include <memory>
#include "../Physics/IPhysicsBody.h"
#include "../Events/GameEvents.h"
#include "../Physics/ICollisionListener.h"
#include "../IIndependentObject.h"

namespace Jam::Domain::Enemy
{
	class Bomb :public Jam::Domain::IIndependentObject
		, public Jam::Domain::Physics::ICollisionListener
		, public std::enable_shared_from_this<Bomb>
	{
	private:
		Texture bombTex;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_explodeBody;

		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		Jam::Domain::Physics::PhysicsBodyID m_playerId;
		double m_damage;
		double m_lifetime;
		double m_timer;
		double m_explodeTimer;
		double m_explodeDuration = 1.5;//爆発の表示時間
		double m_exlosionRadius;
		bool m_isExploding = false;
		bool m_isHit = false;
		Vec2 m_size;
		Vec2 m_scaled = Vec2{ 0.0015 ,0.0015 };
		double hitRatio = 0.6;//当たり判定を先に消す

	public:
		Bomb(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId, Jam::Domain::Events::GameEventQueue& queue, double damage, double explosionRadius, double lifetime, Vec2 size);
		~Bomb();

		void init();
		void update(double deltaTime)override;
		void draw() const override;

		void explosion();

		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
	};
}
