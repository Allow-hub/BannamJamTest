#pragma once
#include "IEnemyAI.h"

namespace Jam::Domain::Enemy
{
	struct ChaseAI : IEnemyAI
	{
		//とりあえず今は直接指定
		//攻撃に移る範囲
		double attackRange = 150.0f;
		//プレイヤーを追いかけられる範囲
		double lostRange = 800.0f;

		void enter(EnemyBase& enemy) override
		{
			enemy.onAIEvent(EnemyAIEvent::Chase);
			// フック
			enemy.onChaseEnter();
		}

		void update(EnemyBase& enemy, double deltaTime) override
		{
			Vec2 enePos = enemy.getPosition();
			Vec2 plPos = enemy.getPlayerPos();
			Vec2 direction = plPos - enePos;
			Vec2 velocity = direction.normalized();

			enemy.getPhysicsBody()->applyForce(velocity * enemy.getStatus().moveSpeed);

			if (direction.length() <= attackRange)
			{
				enemy.onAIEvent(EnemyAIEvent::ReachedGoal);
			}
			else if (direction.length() >= lostRange)
			{
				enemy.onAIEvent(EnemyAIEvent::PlayerLost);
			}
			// フック
			enemy.onChaseUpdate(deltaTime);
		}

		void exit(EnemyBase& enemy) override
		{
			// フック
			enemy.onChaseExit();
		}
	};
}
