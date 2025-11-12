#pragma once
#include "IChokerState.h"
#include "../../../../Presentation/AudioService.h"
#include "HookedGroundState.h"
#include "HookedEnemyState.h"

using namespace Jam::Domain::Physics;

namespace Jam::Domain::Player
{
	class ChokerFlyingState : public IChokerState
	{
	private:
		Texture wireTex;
		const Vec2 createOffset = Vec2{ 50, -30 };
		const double maxFlyTime = 0.15; // 最大飛行時間
		double flyTimer = 0.0;

	public:
		ChokerFlyingState()
			: wireTex(U"Assets/Player/wire.png")
		{
		}

		void enter(ChokerContext& ctx) override
		{
			Jam::Presentation::AudioService::get().playOneShot(Jam::Presentation::AudioService::Sound::SE_Choker, 0.3);

			Vec2 target = Jam::Infrastructure::CursorUtil::instance().getCursorPosF();
			bool isRight = (target.x >= ctx.player.getPosition().x);
			Vec2 offset = isRight ? createOffset : Vec2{ -createOffset.x, createOffset.y };
			Vec2 hookStartPos = ctx.player.getPosition() + offset;

			ctx.body->setPos(hookStartPos);

			Vec2 diff = target - hookStartPos;
			if (diff.isZero()) diff = Vec2{ 1, 0 };
			Vec2 dir = diff.normalized();

			const double speed = 7000.0;
			ctx.body->setVelocity(dir * speed);

			ctx.isActive = true;
			ctx.isFlying = true;
			flyTimer = 0.0;
		}

		void update(ChokerContext& ctx, double deltaTime) override
		{
			flyTimer += deltaTime;

			if (flyTimer >= maxFlyTime)
			{
				// 一定時間経過でヒットなしなら Idle に戻す
				ctx.skill.transitionTo<ChokerIdleState>();
			}
		}

		void draw(const ChokerContext& ctx) const override
		{
			const Vec2 playerPos = ctx.player.getPosition();
			const Vec2 hookPos = ctx.body->getPosition();
			drawWire(playerPos, hookPos, wireTex);

			ctx.body->drawFrame(3.0, Palette::Violet);
		}

		void onCollisionEnter(ChokerContext& ctx, std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body) override
		{
			if (body->getLayer() == PhysicsLayer::Ground)
			{
				ctx.ground = body;
				ctx.skill.transitionTo<HookedGroundState>();
			}
			else if (body->getLayer() == PhysicsLayer::Enemy)
			{
				ctx.targetEnemy = body;
				ctx.skill.transitionTo<HookedEnemyState>();
			}
			else if (body->getLayer() == PhysicsLayer::Wall)
			{
				ctx.body->setVelocity({ 0, 0 });
			}
		}
	};
}
