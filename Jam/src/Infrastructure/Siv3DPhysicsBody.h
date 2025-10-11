#pragma once
#include <Siv3D.hpp>
#include "../Domain/Physics/IPhysicsBody.h"

namespace Jam::Infrastructure::Physics
{
	class Siv3DPhysicsBody : public Jam::Domain::Physics::IPhysicsBody
	{
	private:
		P2Body m_body;
		bool m_grounded = false;

	public:
		Siv3DPhysicsBody(P2World& world, const Vec2& pos,
						 const SizeF& size = SizeF{ 40, 80 },
						 s3d::P2BodyType bodyType = s3d::P2BodyType::Dynamic)
			: m_body(world.createRect(bodyType, pos, size))
		{
		}


		void applyForce(const Vec2& force) override
		{
			m_body.applyForce(force);
		}

		void applyImpulse(const Vec2& impulse)
		{
			m_body.applyLinearImpulse(impulse);
		}

		void setVelocity(const Vec2& v) override
		{
			m_body.setVelocity(v);
		}

		Vec2 getVelocity() const override
		{
			return m_body.getVelocity();
		}

		Jam::Domain::Physics::PhysicsTransform getTransform() const override
		{
			return {
				.position = m_body.getPos(),
				.rotation = m_body.getAngle()
			};
		}

		void setTransform(const Jam::Domain::Physics::PhysicsTransform& t) override
		{
			m_body.setPos(t.position);
			m_body.setAngle(t.rotation);
		}

		bool isGrounded() const override
		{
			return m_grounded; // TODO: 判定は接触コールバックで更新予定
		}
	};
}
