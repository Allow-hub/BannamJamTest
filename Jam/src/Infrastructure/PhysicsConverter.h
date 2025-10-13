#pragma once
#include <Siv3D.hpp>
#include "../Domain/Physics/PhysicsTypes.h"

namespace Jam::Infrastructure::Physics
{
	// 引数順は density, friction, restitution の順
	inline P2Material ToSiv3DMaterial(const Jam::Domain::Physics::PhysicsMaterial& m)
	{
		return P2Material(m.density, m.friction, m.restitution,m.restitutionThreshold);
	}

	inline Jam::Domain::Player::PlayerStats LoadFromJSON(const FilePath& path)
	{
		const JSON json = JSON::Load(path);
		if (!json) throw Error(U"Failed to load Player JSON: {}"_fmt(path));

		Jam::Domain::Player::PlayerStats stats;
		stats.moveSpeed = json[U"stats"][U"moveSpeed"].get<double>();
		stats.jumpPower = json[U"stats"][U"jumpPower"].get<double>();

		const auto& physics = json[U"physics"];
		stats.physicsMaterial = {
			.friction = physics[U"friction"].get<double>(),
			.restitution = physics[U"restitution"].get<double>(),
			.density = physics[U"density"].get<double>(),
			.restitutionThreshold = physics[U"restitutionThreshold"].get<double>(),
		};

		return stats;
	}
}
