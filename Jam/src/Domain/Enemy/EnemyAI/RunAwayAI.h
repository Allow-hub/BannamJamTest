#pragma once
#include "IEnemyAI.h"
#include "../../Physics/IPhysicsBody.h"

namespace Jam::Domain::Enemy
{
	struct RunAwayAI : IEnemyAI
	{
		const Duration RunTime = SecondsF(6.5);
		const Duration JumpInterval = SecondsF(1.5); // ジャンプ間隔

		Timer runTimer{ RunTime, StartImmediately::No };
		Timer jumpTimer{ JumpInterval, StartImmediately::No };

		void enter(EnemyBase& enemy) override
		{
			runTimer.start();
			jumpTimer.start();

			enemy.onChaseEnter();
		}

		void update(EnemyBase& enemy, double deltaTime) override
		{
			auto& setting = enemy.getChaseSettings();
			Vec2 enePos = enemy.getPosition();
			Vec2 plPos = enemy.getPlayerPos();

			Vec2 direction = plPos - enePos;
			Vec2 velocity = direction.normalized();

			// プレイヤーと逆方向に移動
			enemy.getPhysicsBody()->applyForce(
				-velocity * enemy.getStatus().moveSpeed * setting.moveSpeedFactor
			);

			// 🔸 一定間隔でジャンプ
			if (jumpTimer.reachedZero())
			{
				enemy.jump();                  // 敵のジャンプ処理を呼ぶ
				jumpTimer.restart();           // タイマーをリセットして再スタート
			}

			// 一定距離または時間で逃走終了
			if (direction.length() >= setting.loseRange * 2 || runTimer.reachedZero())
			{
				enemy.onAIEvent(EnemyAIEvent::PlayerLost);
			}

			enemy.onChaseUpdate(deltaTime);
		}

		void exit(EnemyBase& enemy) override
		{
			runTimer.reset();
			jumpTimer.reset();

			enemy.onChaseExit();
		}
	};
}
