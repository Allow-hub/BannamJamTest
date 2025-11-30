#pragma once
#include "Domain/Player/Choker/ChokerStates/IChokerState.h"

namespace Jam::Domain::Player
{
	class ChokerIdleState : public IChokerState
	{
	public:
		void enter(ChokerContext& ctx) override
		{
			// フックをリセット
			ctx.skill.releaseJoint();
			ctx.skill.resetHook();
			ctx.isActive = false;
			ctx.isFlying = false;
			ctx.isHooked = false;
			ctx.joint.reset();
			ctx.lastDir = Vec2{ 0, 0 };

			if (ctx.body)
			{
				ctx.body->setPos(Vec2{ -10000, -10000 });
				ctx.body->setVelocity(Vec2::Zero());
				ctx.body->setAngularVelocity(0);
			}
		}

		void update(ChokerContext& ctx, double deltaTime) override
		{
		}
	};
}
