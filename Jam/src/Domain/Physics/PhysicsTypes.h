#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Physics
{
	struct PhysicsTransform
	{
		Vec2 position;
		double rotation = 0.0;
	};

	//物理マテリアル
	struct PhysicsMaterial
	{
		double friction = 0.2;
		double restitution = 0.0;
		double density = 1.0;
		double restitutionThreshold = 1.0;
	};

	enum class PhysicsShape
	{
		Rect,//矩形
		Circle,

	};

	enum class PhysicsType
	{
		Static,
		Kinematic,
		Dynamic
	};

	//物理の判定用レイヤー
	enum class PhysicsLayer
	{
		None,
		Player,
		Enemy,
		Ground,
		Weapon,
		Wall,
	};
}