#pragma once
#include "IEnemyAI.h"
#include "../../Physics/IPhysicsBody.h"

namespace Jam::Domain::Enemy
{

	struct PatrolPoint {
		Vec2 pos;
	};

	struct PatrolAIExtra {
		Array<PatrolPoint> patrolPoints;
		bool loop = false;
		double waitTime = 0.0;
	};

	struct PatrolRoute {
		Array<Vec2> points;
		bool loop = false;
		double waitTime = 0.0;
		size_t currentIndex = 0;
		double waitTimer = 0.0;

		bool isValid() const { return not points.isEmpty(); }

		void advance() {
			if (points.isEmpty()) return;
			currentIndex++;
			if (currentIndex >= points.size()) {
				currentIndex = loop ? 0 : (points.size() - 1);
			}
		}

		void resetTimer() { waitTimer = 0; }
		void updateTimer(double dt) { waitTimer += dt; }
		bool isWaitOver() const { return waitTimer >= waitTime; }
	};

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
