#pragma once
#include "../../Physics/IPhysicsBody.h"
#include "../../Physics/ICollisionListener.h"

namespace Jam::Domain::Player
{
	class BombEntity : public Jam::Domain::Physics::ICollisionListener
	{
	private:
		std::unique_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		double m_timer = 0.0;
		double m_lifeTime = 3.0; // 爆発までの時間
		bool m_exploded = false;

	public:
		BombEntity(std::unique_ptr<Jam::Domain::Physics::IPhysicsBody> body)
			: m_body(std::move(body)) {
		}

		bool isAlive() const { return !m_exploded; }

		void update(double deltaTime)
		{
			if (m_exploded) return;

			m_timer += deltaTime;

			// 時間経過で爆発
			if (m_timer >= m_lifeTime)
			{
				explode();
			}

		}

		void explode()
		{
			if (m_exploded) return;
			m_exploded = true;
			Print << U"💥 Bomb exploded!";

		}

		s3d::Vec2 getPos() const
		{
			return m_body->getPosition();
		}

		// ICollisionListener
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override
		{
			if (other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Enemy)
			{
				Print << U"Bomb hit Enemy";
				explode();
			}
		}

		//特に処理なし
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override{}
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override{}
	};
}
