#pragma once
#include "IEnemyAI.h"

namespace Jam::Domain::Enemy
{
	//標的を探して一定位置を移動
	struct PatrolAI : IEnemyAI
	{
		void enter(EnemyBase& enemy) override
		{
			enemy.onAIEvent(EnemyAIEvent::PlayerFound);
			// フック
			enemy.onPatrolEnter();
		}

		void update(EnemyBase& enemy, double deltaTime) override
		{
			// フック
			enemy.onPatrolUpdate(deltaTime);
		}

		void exit(EnemyBase& enemy) override
		{
			Print << U"EXIT";
			// フック
			enemy.onPatrolExit();
		}
	};
}
