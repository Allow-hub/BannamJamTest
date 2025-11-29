#pragma once
#include "Domain/Enemy/EnemyAI/IEnemyAI.h"

namespace Jam::Domain::Enemy
{
	struct ChaseAI : IEnemyAI
	{
		//とりあえず今は直接指定
		//攻撃に移る範囲
		//double attackRange = 150.0f;
		//プレイヤーを追いかけられる範囲
		//double lostRange = 800.0f;

		void enter(EnemyBase& enemy) override
		{
			enemy.onAIEvent(EnemyAIEvent::Chase);
			// フック
			enemy.onChaseEnter();
		}

		void update(EnemyBase& enemy, double deltaTime) override
		{
			auto& setting = enemy.getChaseSettings();
			Vec2 enePos = enemy.getPosition();
			Vec2 plPos = enemy.getPlayerPos();
			Vec2 direction = plPos - enePos;
			Vec2 velocity = direction.normalized();

			if(direction.x < 0)
				enemy.setFaceLeft(true);
			else
				enemy.setFaceLeft(false);

			enemy.getPhysicsBody()->applyForce(velocity * enemy.getStatus().moveSpeed * setting.moveSpeedFactor);

			if (direction.length() <= setting.attackRange)
			{
				enemy.onAIEvent(EnemyAIEvent::ReachedGoal);
			}
			else if (direction.length() >= setting.loseRange)
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
