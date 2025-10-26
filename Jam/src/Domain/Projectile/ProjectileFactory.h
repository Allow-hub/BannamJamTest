# pragma once
# include <Siv3D.hpp>
# include "ProjectileBase.h"
# include "ProjectileType.h"

class ProjectileFactory
{
public:
	std::unique_ptr<ProjectileBase> create(
		ProjectileType type,
		P2World& world,
		const Vec2& enemyPos,
		const Vec2& playerPos
	);
};
