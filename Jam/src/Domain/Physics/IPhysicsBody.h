#pragma once
#include "PhysicsTypes.h"

namespace Jam::Domain::Physics
{

	class IPhysicsBody
	{
	public:
		virtual ~IPhysicsBody() = default;

		virtual void applyForce(const Vec2& force) = 0;
		virtual void applyImpulse(const Vec2& impulse) = 0;
		virtual void setVelocity(const Vec2& velocity) = 0;
		virtual Vec2 getVelocity() const = 0;

		virtual PhysicsTransform getTransform() const = 0;
		Vec2 getPosition() const
		{
			return getTransform().position;
		}
		virtual void setTransform(const PhysicsTransform& t) = 0;
		virtual void setPos(const Vec2& p) = 0;
		virtual void setGravityScale(const double& s) = 0;
		virtual PhysicsLayer getLayer() const = 0;
		virtual void setLayer(PhysicsLayer layer) = 0;
		virtual void drawFrame(const double thickness = 1.0, const ColorF& color = Palette::White) = 0;
	};
}
