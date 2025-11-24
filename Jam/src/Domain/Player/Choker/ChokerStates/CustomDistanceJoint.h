#pragma once
#include "../../../Physics/IPhysicsBody.h"
namespace Jam::Domain::Player
{
	struct CustomDistanceJoint
	{
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> bodyA;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> bodyB;

		double targetLength = 100.0;    // 目標距離
		double stiffness = 50.0;        // バネ定数
		double damping = 5.0;           // 減衰

		bool canStretch = true;         // 目標距離より伸びることを許可
		bool canCompress = true;        // 目標距離より縮むことを許可

		void update(double dt)
		{
			if (!bodyA || !bodyB) return;

			Vec2 delta = bodyB->getPosition() - bodyA->getPosition();
			double distance = delta.length();
			if (distance == 0) return;

			Vec2 dir = delta / distance;
			double diff = distance - targetLength;

			// 伸縮制御
			if ((!canStretch && diff > 0) || (!canCompress && diff < 0))
				diff = 0;

			Vec2 relVel = bodyB->getVelocity() - bodyA->getVelocity();

			// 力の計算（バネ + 減衰）
			Vec2 force = stiffness * diff * dir + damping * relVel;

			// 力を両方に加える
			bodyA->applyForce(force);
			bodyB->applyForce(-force);
		}

		void setTargetLength(double length) { targetLength = length; }
		void setStiffness(double k) { stiffness = k; }
		void setDamping(double d) { damping = d; }
		void setStretchable(bool stretch) { canStretch = stretch; }
		void setCompressible(bool compress) { canCompress = compress; }
	};
}
