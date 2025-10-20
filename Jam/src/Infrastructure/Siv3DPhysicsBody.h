#pragma once
#include <Siv3D.hpp>
#include "../Domain/Physics/IPhysicsBody.h"
#include "PhysicsConverter.h"
#include "../Infrastructure/PhysicsFilterManager.h"

namespace Jam::Infrastructure::Physics
{
	class Siv3DPhysicsBody : public Jam::Domain::Physics::IPhysicsBody
	{
	private:
		P2Body m_body;
		std::weak_ptr<Jam::Domain::Physics::ICollisionListener> m_listener;
		Jam::Domain::Physics::PhysicsLayer m_layer = Jam::Domain::Physics::PhysicsLayer::None;

	public:
		Siv3DPhysicsBody(P2World& world,
						 const Vec2& pos,
						 const SizeF& size = SizeF{ 40, 80 },
						 s3d::P2BodyType bodyType = s3d::P2BodyType::Dynamic,
						 const Jam::Domain::Physics::PhysicsMaterial& material = Jam::Domain::Physics::PhysicsMaterial{ 0.2 ,0.0,1.0 },
						 const Jam::Domain::Physics::PhysicsShape shape = Jam::Domain::Physics::PhysicsShape::Rect)
		{
			switch (shape)
			{
			case Jam::Domain::Physics::PhysicsShape::Rect:
				m_body = world.createRect(bodyType, pos, size, ToSiv3DMaterial(material));
				break;
			case Jam::Domain::Physics::PhysicsShape::Circle:
				m_body = world.createCircle(bodyType, pos, size.x * 0.5, ToSiv3DMaterial(material));
				break;
			default:
				m_body = world.createRect(bodyType, pos, size, ToSiv3DMaterial(material));
				break;
			}
			m_body.setDamping(2.0);
			m_body.setAngularDamping(2.0);
			m_body.setFixedRotation(true);
			m_body.setSleepEnabled(true);
		}

		void applyForce(const Vec2& force) override { m_body.applyForce(force); }
		void applyImpulse(const Vec2& impulse) { m_body.applyLinearImpulse(impulse); }
		void setVelocity(const Vec2& v) override { m_body.setVelocity(v); }
		void setAngularVelocity(const double& vel) override { m_body.setAngularVelocity(vel); }
		Vec2 getVelocity() const override { return m_body.getVelocity(); }
		void setPos(const Vec2& p)override { m_body.setPos(p); }
		void setBullet(const bool b) override { m_body.setBullet(b); }
		void setLayer(Jam::Domain::Physics::PhysicsLayer layer) override { m_layer = layer; }
		void drawFrame(const double thickness = 1.0, const ColorF& color = Palette::White) { m_body.drawFrame(thickness, color); }
		Jam::Domain::Physics::PhysicsLayer getLayer() const override { return m_layer; }
		void* getNativeBody() override { return &m_body; }
		void setBodyType(Jam::Domain::Physics::PhysicsType type) override { m_body.setBodyType(ToSiv3DBodyType(type)); }
		void setFilter(Jam::Infrastructure::PhysicsFilter filter) override { m_body.shape(0).setFilter(GetFilter(filter)); }

		void addCircleSensor(const s3d::Circle& localPos, const Jam::Infrastructure::PhysicsFilter& filter) override
		{
			m_body.addCircleSensor(localPos,Jam::Infrastructure::GetFilter(filter));
		}

		std::optional<P2DistanceJoint> createDistanceJoint(
			P2World& world,
			const std::shared_ptr<IPhysicsBody>& other,
			const Vec2& anchorThis,
			const Vec2& anchorOther,
			double length
		) override
		{
			auto otherSiv = std::dynamic_pointer_cast<Siv3DPhysicsBody>(other);
			if (!otherSiv) return std::nullopt;

			auto joint = world.createDistanceJoint(
				m_body,
				anchorThis,
				otherSiv->m_body,
				anchorOther,
				length,
				EnableCollision::No
			);

			return joint;
		}
		Jam::Domain::Physics::PhysicsBodyID getID() const override
		{
			return static_cast<Jam::Domain::Physics::PhysicsBodyID>(m_body.id());
		}

		Jam::Domain::Physics::PhysicsTransform getTransform() const override
		{
			return { .position = m_body.getPos(), .rotation = m_body.getAngle() };
		}

		void setTransform(const Jam::Domain::Physics::PhysicsTransform& t) override
		{
			m_body.setPos(t.position);
			m_body.setAngle(t.rotation);
		}

		void setGravityScale(const double& s) { m_body.setGravityScale(s); }

		[[nodiscard]]
		P2BodyID getBodyID() const noexcept { return m_body.id(); }
		const P2Body& getBody() const { return m_body; }
		P2Body& getBody()  { return m_body; }

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
