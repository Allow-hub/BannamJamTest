#pragma once
#include <Siv3D.hpp>
#include "../Domain/Physics/PhysicsTypes.h"

namespace Jam::Infrastructure::Physics
{
	// 引数順は density, friction, restitution の順
	inline P2Material ToSiv3DMaterial(const Jam::Domain::Physics::PhysicsMaterial& m)
	{
		return P2Material(m.density, m.friction, m.restitution);
	}
}
