#pragma once
#include "IEnemyAI.h"

namespace Jam::Domain::Enemy
{
	struct RunAwayAI : IEnemyAI
	{
		//とりあえず今は直接指定
		//攻撃に移る範囲
		//double attackRange = 150.0f;
		//プレイヤーを追いかけられる範囲
		//double lostRange = 800.0f;

		//逃げる時間(5秒間)
		const Duration RunTime = SecondsF(7.5);
		//タイマー
		Timer runTimer{ RunTime, StartImmediately::No };

		void enter(EnemyBase& enemy) override
		{
			//タイマー計測開始
			runTimer.start();

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

			enemy.getPhysicsBody()->applyForce(-velocity * enemy.getStatus().moveSpeed * setting.moveSpeedFactor);

			if (direction.length() >= setting.loseRange * 2 || runTimer.reachedZero())	//ToDoタイマーを置き換える
			{
				enemy.onAIEvent(EnemyAIEvent::PlayerLost);
			}
			// フック
			enemy.onChaseUpdate(deltaTime);
		}

		void exit(EnemyBase& enemy) override
		{
			//タイマーをリセット
			runTimer.reset();
			// フック
			enemy.onChaseExit();
		}
	};
}
