#pragma once
#include "IEnemyAI.h"
#include "../../Physics/IPhysicsBody.h"

namespace Jam::Domain::Enemy
{
	//==========================================================
	// ■ RunAwayAI
	// プレイヤーから一定時間 or 一定距離まで離れるAI。
	// 追跡AI(ChaseAI)とは逆方向に移動し、時間または距離条件で終了する。
	//==========================================================
	struct RunAwayAI : IEnemyAI
	{
		// 逃げる持続時間
		const Duration RunTime = SecondsF(7.5);

		// タイマー（開始時に手動でstart()を呼ぶ）
		Timer runTimer{ RunTime, StartImmediately::No };

		void enter(EnemyBase& enemy) override
		{
			// タイマー計測開始
			runTimer.start();

			enemy.onChaseEnter();
		}

		void update(EnemyBase& enemy, double deltaTime) override
		{
			// プレイヤーとの相対位置を取得
			auto& setting = enemy.getChaseSettings();
			Vec2 enePos = enemy.getPosition();
			Vec2 plPos = enemy.getPlayerPos();

			// プレイヤーへの方向ベクトルを算出
			Vec2 direction = plPos - enePos;

			// 逃げる方向はプレイヤーとは逆方向
			Vec2 velocity = direction.normalized();

			// プレイヤーと反対方向へ力を加える（逃走挙動）
			enemy.getPhysicsBody()->applyForce(
				-velocity * enemy.getStatus().moveSpeed * setting.moveSpeedFactor
			);

			// 一定距離離れるか、逃走時間が終了したらAIイベントを送信
			if (direction.length() >= setting.loseRange * 2 || runTimer.reachedZero())
			{
				enemy.onAIEvent(EnemyAIEvent::PlayerLost);
			}

			enemy.onChaseUpdate(deltaTime);
		}

		void exit(EnemyBase& enemy) override
		{
			runTimer.reset();

			enemy.onChaseExit();
		}
	};
}
