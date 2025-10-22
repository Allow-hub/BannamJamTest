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
			if (!route.isValid()) return;

			auto currentTarget = route.points[route.currentIndex];
			Vec2 pos = enemy.getPosition();
			Vec2 dir = (currentTarget - pos).normalized();

			enemy.getPhysicsBody()->applyForce(dir * enemy.getStatus().moveSpeed);

			if (pos.distanceFrom(currentTarget) < 5.0) // 到達判定
			{
				route.updateTimer(deltaTime);
				if (route.isWaitOver()) {
					route.advance();
					route.resetTimer();
				}
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
