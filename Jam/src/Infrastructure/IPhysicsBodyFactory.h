#pragma once
#include <memory>
#include "../Domain/Physics/IPhysicsBody.h"
#include <Siv3D.hpp>

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
	};
}
