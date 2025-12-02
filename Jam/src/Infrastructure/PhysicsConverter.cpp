#include "Infrastructure/PhysicsConverter.h"

namespace Jam::Infrastructure::Physics
{
	Jam::Domain::Player::PlayerStats LoadFromJSON(const FilePath& path)
	{
		const JSON json = JSON::Load(path);
		if (!json) throw Error(U"Failed to load Player JSON: {}"_fmt(path));

		Jam::Domain::Player::PlayerStats stats;
		stats.hp = json[U"stats"][U"hp"].get<double>();
		stats.power = json[U"stats"][U"power"].get<double>();
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