#pragma once
#include "IPhysicsBodyFactory.h"
#include "Siv3DPhysicsBody.h"

namespace Jam::Infrastructure::Locator
{
	class Siv3DPhysicsBodyFactory : public IPhysicsBodyFactory
	{
	private:
		P2World* m_world = nullptr;

	public:
		Siv3DPhysicsBodyFactory() = default;

		void initialize(P2World& world)
		{
			m_world = &world;
		}

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> createBody(
			const Vec2& pos,
			const SizeF& size = SizeF{ 40, 80 },
			s3d::P2BodyType bodyType = s3d::P2BodyType::Dynamic,
			const Jam::Domain::Physics::PhysicsMaterial& material = { 0.2, 0.0, 1.0 },
			const Jam::Domain::Physics::PhysicsShape shape = Jam::Domain::Physics::PhysicsShape::Rect
		) override
		{
			if (!m_world)
				throw std::runtime_error("Siv3DPhysicsBodyFactory not initialized!");

			return std::make_shared<Jam::Infrastructure::Physics::Siv3DPhysicsBody>(*m_world, pos, size, bodyType, material, shape);
		}
	};
}
