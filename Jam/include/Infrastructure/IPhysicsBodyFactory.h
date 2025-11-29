#pragma once
#include <memory>
#include "Domain/Physics/IPhysicsBody.h"
#include <Siv3D.hpp>
#include "Domain/Physics/PhysicsBodyID.h"

namespace Jam::Infrastructure::Locator
{
	class IPhysicsBodyFactory
	{
	public:
		virtual ~IPhysicsBodyFactory() = default;

		virtual std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> createBody(
			const Vec2& pos,
			const SizeF& size = SizeF{ 40, 80 },
			s3d::P2BodyType bodyType = s3d::P2BodyType::Dynamic,
			const Jam::Domain::Physics::PhysicsMaterial& material = { 0.2, 0.0, 1.0 },
			const Jam::Domain::Physics::PhysicsShape shape = Jam::Domain::Physics::PhysicsShape::Rect) = 0;

		virtual std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> createCircleSensor(
			const Vec2& pos,
			double radius,
			s3d::P2BodyType bodyType = s3d::P2BodyType::Static
		) = 0;

		virtual std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> createRectSensor(
			const Vec2& pos,
			const SizeF& size,
			s3d::P2BodyType bodyType = s3d::P2BodyType::Static
		) = 0;

		virtual void removeBody(Jam::Domain::Physics::PhysicsBodyID id) = 0;
		virtual std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> getBody(Jam::Domain::Physics::PhysicsBodyID id) const = 0;
		virtual P2World& getWorld() const = 0;
	};
}
