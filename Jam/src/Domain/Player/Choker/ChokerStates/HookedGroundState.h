#pragma once
#include "IChokerState.h"

using namespace Jam::Domain::Physics;

namespace Jam::Domain::Player
{
	class HookedGroundState : public IChokerState
	{
	private:
		Texture wireTex;
		const double m_minDist = 1.0;
		const double m_maxDist = 400.0; // 上限
		const double shrinkSpeed = 2000.0;
		
	public:
		HookedGroundState()
			: wireTex(U"Assets/Player/wire.png")
		{
		}

		void enter(ChokerContext& ctx) override
		{
			if (ctx.isFlying)
			{
				ctx.eventQueue.push(Events::PlayerChokerSkillEvent{ 0.8, 0.7 });
				ctx.body->setVelocity(Vec2::Zero());
				ctx.body->setAngularVelocity(0);
				ctx.body->setBodyType(PhysicsType::Static);
				ctx.isFlying = false;

				auto& world = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
					.getPhysicsFactory()->getWorld();

				Vec2 hookPos = ctx.body->getPosition();
				auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
					.getPhysicsFactory()->getBody(ctx.ownerId);

				if (!playerBody)
					return;

				Vec2 playerPos = playerBody->getPosition();
				double dist = (hookPos - playerPos).length();
				dist = std::clamp(dist, m_minDist, 9999.0);

				ctx.skill.releaseJoint();

				ctx.joint = ctx.body->createDistanceJoint(world, playerBody, hookPos, playerPos, dist);

				if (ctx.joint.has_value())
				{
					ctx.joint->setLinearStiffness(10.0, 1.0);
					ctx.joint->setMinLength(0.0);
					ctx.joint->setMaxLength(dist);

					Vec2 diff = hookPos - playerPos;
					if (diff.isZero()) diff = Vec2{ 0, -1 };
					ctx.lastDir = diff.normalized().rotated(-8_deg);

					if (ctx.ground)
					{
						Vec2 groundPos = ctx.ground->getPosition();
						ctx.groundAnchorOffset = hookPos - groundPos;
					}
				}
				else
				{
					Print << U"⚠️ ジョイント作成失敗";
				}
			}
		}

		void update(ChokerContext& ctx, double deltaTime) override
		{
			if (!ctx.joint.has_value())
				return;

			if (ctx.ground)//動く床に追従
			{
				Vec2 groundPos = ctx.ground->getPosition();
				ctx.body->setPos(groundPos + ctx.groundAnchorOffset);
			}

			double currentMax = ctx.joint->getMaxLength();

			// 縮める（m_maxDist 以下にはしない）
			double newMax = currentMax - shrinkSpeed * deltaTime;
			if (newMax < m_maxDist)
				newMax = m_maxDist;

			ctx.joint->setMaxLength(newMax);
		}

		void draw(const ChokerContext& ctx) const override
		{
			if (!ctx.joint.has_value()) return;

			auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()->getBody(ctx.ownerId);
			if (!playerBody) return;

			const auto playerPos = playerBody->getPosition();
			const auto hookPos = ctx.body->getPosition();

			drawWire(playerPos, hookPos, wireTex);

			ctx.body->drawFrame(3.0, Palette::Violet);
		}



		bool isHookedGround() const override { return true; }
	};
}
