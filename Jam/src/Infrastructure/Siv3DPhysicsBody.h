#pragma once
#include <Siv3D.hpp>
#include "../Domain/Physics/IPhysicsBody.h"
#include "PhysicsConverter.h"

namespace Jam::Infrastructure::Physics
{
	class Siv3DPhysicsBody : public Jam::Domain::Physics::IPhysicsBody
	{
	private:
		P2Body m_body;
		std::weak_ptr<Jam::Domain::Physics::ICollisionListener> m_listener;
		Jam::Domain::Physics::PhysicsLayer m_layer = Jam::Domain::Physics::PhysicsLayer::None;

	public:
		Siv3DPhysicsBody(P2World& world, const Vec2& pos,
						 const SizeF& size = SizeF{ 40, 80 },
						 s3d::P2BodyType bodyType = s3d::P2BodyType::Dynamic,
						 const Jam::Domain::Physics::PhysicsMaterial& material = Jam::Domain::Physics::PhysicsMaterial{ 0.2 ,0.0,1.0 })
			: m_body(world.createRect(bodyType, pos, size, Jam::Infrastructure::Physics::ToSiv3DMaterial(material)))
		{
			m_body.setDamping(2.0);
			m_body.setAngularDamping(2.0);
			m_body.setFixedRotation(true);
			m_body.setSleepEnabled(true);
		}

		void applyForce(const Vec2& force) override { m_body.applyForce(force); }
		void applyImpulse(const Vec2& impulse) { m_body.applyLinearImpulse(impulse); }
		void setVelocity(const Vec2& v) override { m_body.setVelocity(v); }
		Vec2 getVelocity() const override { return m_body.getVelocity(); }
		void setLayer(Jam::Domain::Physics::PhysicsLayer layer) override { m_layer = layer; }
		Jam::Domain::Physics::PhysicsLayer getLayer() const override { return m_layer; }


		Jam::Domain::Physics::PhysicsTransform getTransform() const override
		{
			return { .position = m_body.getPos(), .rotation = m_body.getAngle() };
		}

		void setTransform(const Jam::Domain::Physics::PhysicsTransform& t) override
		{
			m_body.setPos(t.position);
			m_body.setAngle(t.rotation);
		}

		[[nodiscard]]
		P2BodyID getBodyID() const noexcept { return m_body.id(); }
		const P2Body& getBody() const { return m_body; }

		void setCollisionListener(const std::shared_ptr<Jam::Domain::Physics::ICollisionListener>& listener)
		{
			m_listener = listener;
		}

		void notifyCollisionEnter(const std::shared_ptr<IPhysicsBody>& other)
		{
			if (auto l = m_listener.lock())
				l->onCollisionEnter(other);
		}

		void notifyCollisionStay(const std::shared_ptr<IPhysicsBody>& other)
		{
			if (auto l = m_listener.lock())
				l->onCollisionStay(other);
		}
		void notifyCollisionExit(const std::shared_ptr<IPhysicsBody>& other)
		{
			if (auto l = m_listener.lock())
				l->onCollisionExit(other);
		}

	};
}
