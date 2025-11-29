#pragma once
#include <Siv3D.hpp>
#include <memory>
#include <unordered_map>
#include "../Domain/Enemy/EnemyBase.h"
#include "../Presentation/EnemyManager.h"
#include "../Domain/Physics/PhysicsTypes.h"
#include "../UseCase/EnemyFactory.h"
#include "../Infrastructure/FactoryServiceLocator.h"
#include "../Infrastructure/IPhysicsBodyFactory.h"
#include "../Domain/Events/GameEvents.h"

namespace Jam::Infrastructure
{
	//敵をロード
	class EnemyLoader
	{
	public:
		//ステージ情報を含んだ敵のロード
		static bool loadEnemyForStageFromJSON(
			const s3d::FilePath& jsonPath,
			const std::unique_ptr<Jam::UseCase::EnemyFactory>& enemyFactory,
			const std::unique_ptr<Jam::Presentation::EnemyManager>& enemyManager,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& eventQueue);

		//敵のステータスをJSONから
		static bool loadEnemyStatusFromJSON(
			const s3d::FilePath& jsonPath,
			std::unordered_map<Jam::Domain::EnemyType, Jam::Domain::Enemy::EnemyStatus>& outTable);
	};
}
