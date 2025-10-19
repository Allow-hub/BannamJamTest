#pragma once
#include "IEnemyAI.h"

namespace Jam::Domain::Enemy
{
	struct ChaseAI : IEnemyAI
	{
		void enter(EnemyBase& enemy) override
		{
			//enemy.onAIEvent(EnemyAIEvent::Chase);
			// フック
			enemy.onChaseEnter();
		}

		void update(EnemyBase& enemy, double deltaTime) override
		{
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
