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

			// 🔸 すでにプレイヤーが発見距離内にいる場合は即イベント発火
			Vec2 playerPos = enemy.getPlayerPos();
			double distanceToPlayer = pos.distanceFrom(playerPos);
			if (distanceToPlayer <= route.foundDistance)
			{
				enemy.onAIEvent(EnemyAIEvent::PlayerFound);
				return; // 即座にChaseAIなどへ遷移させたい場合
			}

			// 巡回目標に到達したら次へ
			if (pos.distanceFrom(currentTarget) < route.foundDistance)
			{
				route.updateTimer(deltaTime);
				if (route.isWaitOver())
				{
					route.advance();
					route.resetTimer();
				}
			}

			enemy.onPatrolUpdate(deltaTime);
		}


		void exit(EnemyBase& enemy) override
		{
			// フック
			enemy.onPatrolExit();
		}
	};
}
