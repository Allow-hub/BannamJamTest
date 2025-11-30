#pragma once
#include <memory>
#include "Domain/Physics/IPhysicsBody.h"
#include "Domain/Player/Choker/ChokerContext.h"

namespace Jam::Domain::Player
{
	class ChokerSkill;
	struct ChokerContext;

	class IChokerState
	{
	public:
		virtual ~IChokerState() = default;
		virtual void enter(ChokerContext& ctx) {}
		virtual void update(ChokerContext& ctx, double deltaTime) = 0;
		virtual void exit(ChokerContext& ctx) {}

		virtual void draw(const ChokerContext& ctx) const {}
		virtual void onCollisionEnter(ChokerContext& ctx, std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body) {}

		// Playerクラスで移動速度に影響を与えるときに使う
		virtual bool isHookedGround() const { return false; }
	};

	inline void drawWire(const s3d::Vec2& start, const s3d::Vec2& end, const s3d::Texture& tex, double thickness = 0.2)
	{
		s3d::Vec2 diff = end - start;
		double len = diff.length();
		if (len <= 0.0) return;

		double angle = Math::Atan2(diff.y, diff.x);
		double scaleX = len / tex.width();
		double scaleY = thickness;
		s3d::Vec2 centerPos = (start + end) * 0.5;

		tex.scaled(scaleX, scaleY)
			.rotated(angle)
			.drawAt(centerPos);
	}
}
