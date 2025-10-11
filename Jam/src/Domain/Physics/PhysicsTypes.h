#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Physics
{
	struct PhysicsTransform
	{
		Vec2 position;
		double rotation = 0.0;
	};

	struct PhysicsMaterial
	{
		double friction = 0.2;
		double restitution = 0.0;
		double density = 1.0;
	};
}
