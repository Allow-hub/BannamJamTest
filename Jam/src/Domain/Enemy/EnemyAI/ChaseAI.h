#pragma once
#include "IEnemyAI.h"

namespace Jam::Domain::Enemy
{
	struct ChaseAI : IEnemyAI
	{
		enum MoveState
		{
			MoveRight = 0,
			MoveLeft
		};
		MoveState moveState;

		void enter(EnemyBase& enemy) override
		{
			//enemy.onAIEvent(EnemyAIEvent::Chase);
			// フック
			enemy.onChaseEnter();
		}

		void update(EnemyBase& enemy, double deltaTime) override
		{
			//とりあえず単純にプレイヤーのいる方向に向かう処理にしています。
			//移動方向切り替え
			if (enemy.getPosition().x <= enemy.getPlayerPos().x)
			{

				moveState = MoveRight;
			}
			else if (enemy.getPosition().x >= enemy.getPlayerPos().x)
			{
				moveState = MoveLeft;
			}

			//移動
			switch (moveState)
			{
			case MoveRight:
				{
					enemy.moveRight();
				}break;
			case MoveLeft:
				{
					enemy.moveLeft();
				}break;
			default:
				break;
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
