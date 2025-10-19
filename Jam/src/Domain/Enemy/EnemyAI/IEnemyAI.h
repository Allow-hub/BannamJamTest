#pragma once
#include "../EnemyBase.h"

namespace Jam::Domain::Enemy
{
	//共通化したエネミーのAI
	//それぞれのAIを具象クラスで組み立てる
	struct IEnemyAI
	{
		virtual ~IEnemyAI() = default;
		virtual void enter(class EnemyBase& enemy) {}
		virtual void update(class EnemyBase& enemy, double deltaTime) = 0;
		virtual void exit(class EnemyBase& enemy) {}
	};

	// AI内で何に切り替えるかは知りたくないのでイベントだけ起こす
	enum class EnemyAIEvent
	{
		None,
		PlayerFound,
		PlayerLost,
		Chase,
		Damaged,
		ReachedGoal,
	};

	//AIの種類
	enum class AIType
	{
		None = 0,
		Patrol,
		Chase,
		Attack
	};

	inline const s3d::String AITypeToString(AIType type)
	{
		switch (type)
		{
		case AIType::Patrol: return U"Patrol";
		case AIType::Chase:  return U"Chase";
		case AIType::Attack: return U"Attack";
		default:             return U"Unknown";
		}
	}

}
