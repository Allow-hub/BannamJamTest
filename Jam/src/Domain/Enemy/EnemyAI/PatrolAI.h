#pragma once
#include "IEnemyAI.h"
#include "../../Physics/IPhysicsBody.h"

namespace Jam::Domain::Enemy
{
	//標的を探して一定位置を移動
	struct PatrolAI : IEnemyAI
	{
		void enter(EnemyBase& enemy) override
		{
			// フック
			enemy.onPatrolEnter();
		}

		void update(EnemyBase& enemy, double deltaTime) override
		{
			auto& route = enemy.getPatrolRoute();
			auto& setting = enemy.getPatrolSettings();
			if (!route.isValid()) return;

			auto currentTarget = route.points[route.currentIndex];
			Vec2 pos = enemy.getPosition();
			Vec2 dir = (currentTarget - pos).normalized();

			enemy.getPhysicsBody()->applyForce(dir * enemy.getStatus().moveSpeed);

			if (pos.distanceFrom(currentTarget) < route.foundDistance) // 到達判定
			{
				route.updateTimer(deltaTime);
				if (route.isWaitOver()) {
					route.advance();
					route.resetTimer();
				}
				enemy.onAIEvent(EnemyAIEvent::PlayerFound);
			}

			Vec2 plPos = enemy.getPlayerPos();
			Vec2 vector = plPos - pos;
			const double distance = vector.length();

			if (distance <= enemy.getPatrolSettings().foundDistance)
			{
				EnemyAIEvent::PlayerFound;
				enemy.onAIEvent(EnemyAIEvent::PlayerFound);
			}

			enemy.onPatrolUpdate(deltaTime); // フック呼び出し
		}


		void exit(EnemyBase& enemy) override
		{
			// フック
			enemy.onPatrolExit();
		}
	};
}
