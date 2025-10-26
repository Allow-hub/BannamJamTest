#pragma once
#include "PhysicsTypes.h"
#include "PhysicsBodyID.h"
#include "../../Infrastructure/PhysicsFilterManager.h"

namespace Jam::Domain::Physics
{
	class ICollisionListener;
	class IPhysicsBody
	{
	public:
		virtual ~IPhysicsBody() = default;

		virtual void applyForce(const Vec2& force) = 0;
		virtual void applyImpulse(const Vec2& impulse) = 0;
		virtual void setVelocity(const Vec2& velocity) = 0;
		virtual void setAngularVelocity(const double& vel) = 0;
		virtual Vec2 getVelocity() const = 0;

		virtual PhysicsTransform getTransform() const = 0;
		Vec2 getPosition() const
		{
			return getTransform().position;
		}
		virtual void setTransform(const PhysicsTransform& t) = 0;
		virtual void setPos(const Vec2& p) = 0;
		virtual void setBullet(const bool b) = 0;
		virtual void setGravityScale(const double& s) = 0;
		virtual void setDamping(const bool b) = 0;
		virtual PhysicsLayer getLayer() const = 0;
		virtual void setLayer(PhysicsLayer layer) = 0;
		virtual void setAngle(double angle) = 0;
		virtual void drawFrame(const double thickness = 1.0, const ColorF& color = Palette::White) = 0;
		virtual void setCollisionListener(const std::shared_ptr<ICollisionListener>& listener) = 0;
		virtual void setBodyType(Jam::Domain::Physics::PhysicsType type) = 0;
		virtual void* getNativeBody() { return nullptr; }//継承先のボディを返す
		virtual void setFilter(Jam::Infrastructure::PhysicsFilter filter) = 0;
		virtual void addCircleSensor(const s3d::Circle& localPos, const Jam::Infrastructure::PhysicsFilter& filter = {}) = 0;
		virtual Jam::Domain::Physics::PhysicsBodyID getID() const = 0;
		virtual std::optional<P2DistanceJoint> createDistanceJoint(
			P2World& world,
			const std::shared_ptr<IPhysicsBody>& other,
			const Vec2& anchorThis,
			const Vec2& anchorOther,
			double length
		) = 0;
		
		/**
		 * すり抜ける床として設定
		 * @param enabled すり抜け床として機能させるか
		 * @param platformTopY 床の上面のY座標（ワールド座標）
		 */
		virtual void setOneWayPlatform(bool enabled, double platformTopY = 0.0) = 0;
	};
}
