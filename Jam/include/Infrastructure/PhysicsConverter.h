#pragma once
#include <Siv3D.hpp>
#include "Domain/Physics/PhysicsTypes.h"
#include "Domain/Player/Player.h"

namespace Jam::Infrastructure::Physics
{
	// 引数順は density, friction, restitution の順
	inline P2Material ToSiv3DMaterial(const Jam::Domain::Physics::PhysicsMaterial& m)
	{
		return P2Material(m.density, m.friction, m.restitution, m.restitutionThreshold);
	}

	Jam::Domain::Player::PlayerStats LoadFromJSON(const FilePath& path);

	inline s3d::P2BodyType ToSiv3DBodyType(Jam::Domain::Physics::PhysicsType type)
	{
		using namespace Jam::Domain::Physics;
		switch (type)
		{
		case PhysicsType::Static:
			return s3d::P2BodyType::Static;
		case PhysicsType::Kinematic:
			return s3d::P2BodyType::Kinematic;
		case PhysicsType::Dynamic:
		default:
			return s3d::P2BodyType::Dynamic;
		}
	}
}
