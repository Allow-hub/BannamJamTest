#pragma once
#include "IEnemyAI.h"

namespace Jam::Domain::Enemy
{
	struct AttackAI : IEnemyAI
	{
		void enter(EnemyBase& enemy) override
		{
			// フック
			enemy.onAttackEnter();
		}

		void update(EnemyBase& enemy, double deltaTime) override
		{
			// フック
			enemy.onAttackUpdate(deltaTime);
		}

		void exit(EnemyBase& enemy) override
		{
			// フック
			enemy.onAttackExit();
		}
	};
}
