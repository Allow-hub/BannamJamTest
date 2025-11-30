#include "Infrastructure/EnemyLoader.h"
#include <cassert>

namespace Jam::Infrastructure
{
	bool EnemyLoader::loadEnemyForStageFromJSON(
		const s3d::FilePath& jsonPath,
		const std::unique_ptr<Jam::UseCase::EnemyFactory>& enemyFactory,
		const std::unique_ptr<Jam::Presentation::EnemyManager>& enemyManager,
		Jam::Domain::Physics::PhysicsBodyID playerId,
		Jam::Domain::Events::GameEventQueue& eventQueue)
	{
		JSON json = JSON::Load(jsonPath);
		assert(json && "Failed to load stage enemy JSON");

		assert(json.getType() == JSONValueType::Array && "JSON root is not an array");

		for (size_t i = 0; i < json.size(); ++i)
		{
			const JSON& item = json[i];
			assert(item.getType() == JSONValueType::Object && "Invalid object in JSON array");

			const String typeStr = item[U"type"].getString();
			const JSON& posJson = item[U"position"];
			const double posX = posJson[U"x"].get<double>();
			const double posY = posJson[U"y"].get<double>();

			Jam::Domain::EnemyType type;
			if (typeStr == U"LittleDevil") type = Jam::Domain::EnemyType::LittleDevil;
			else if (typeStr == U"Ribbon") type = Jam::Domain::EnemyType::Ribbon;
			else if (typeStr == U"GothicLolitaDoll") type = Jam::Domain::EnemyType::GothicLolitaDoll;
			else if (typeStr == U"Spider") type = Jam::Domain::EnemyType::Spider;
			else if (typeStr == U"Eye") type = Jam::Domain::EnemyType::Eye;
			else if (typeStr == U"Clown") type = Jam::Domain::EnemyType::Clown;
			else if (typeStr == U"Boss1_3") type = Jam::Domain::EnemyType::Boss1_3;
			else assert(false && "Unknown enemy type");

			const auto& statusTable = enemyFactory->getStatusTable();
			auto it = statusTable.find(type);
			assert(it != statusTable.end() && "Enemy status not found");

			const Jam::Domain::Enemy::EnemyStatus& status = it->second;

			// Body 作成
			auto enemyBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()
				->createBody(
					Vec2{ posX, posY },
					status.colSize,
					s3d::P2BodyType::Dynamic,
					status.physicsMaterial
				);
			enemyBody->setLayer(Jam::Domain::Physics::PhysicsLayer::Enemy);

			auto enemy = enemyFactory->createEnemy(type, enemyBody, playerId, eventQueue);
			assert(enemy && "Failed to create enemy");

			// extra情報
			if (item.hasElement(U"extra"))
			{
				const JSON& extra = item[U"extra"];
				const JSON& ai = extra[U"ai"];

				if (ai.hasElement(U"patrol"))
				{
					const JSON& patrol = ai[U"patrol"];
					Jam::Domain::Enemy::PatrolRoute route;
					for (const auto& p : patrol[U"patrolPoints"].arrayView())
					{
						route.points << Vec2{ p[U"x"].get<double>(), p[U"y"].get<double>() };
					}
					route.loop = patrol[U"loop"].getOr<bool>(false);
					route.waitTime = patrol[U"waitTime"].getOr<double>(0.0);
					route.foundDistance = patrol[U"foundDistance"].getOr<double>(300.0);
					enemy->setPatrolRoute(route);
				}

				if (ai.hasElement(U"chase"))
				{
					const JSON& chase = ai[U"chase"];
					Jam::Domain::Enemy::ChaseAISettings chaseSettings;

					chaseSettings.attackRange = chase[U"attackRange"].getOr<double>(100.0);
					chaseSettings.loseRange = chase[U"loseRange"].getOr<double>(400.0);
					chaseSettings.moveSpeedFactor = chase[U"moveSpeedFactor"].getOr<double>(1.2);

					enemy->setChaseSettings(chaseSettings);
				}
			}

			enemyBody->setCollisionListener(enemy);
			int enemyId = enemyManager->AddEnemy(enemy, U"Assets/Enemy/" + typeStr + U"/" + typeStr + U"_animation.json");
			enemyManager->getAnimator(enemyId).addCondition({ { {U"isRunning", false} }, U"Idle", 0 });
			enemyManager->getAnimator(enemyId).setBool(U"isRunning", false);
		}

		return true;
	}

	bool EnemyLoader::loadEnemyStatusFromJSON(
		const FilePath& jsonPath,
		std::unordered_map<Jam::Domain::EnemyType, Jam::Domain::Enemy::EnemyStatus>& outTable)
	{
		JSON json = JSON::Load(jsonPath);
		assert(json && "Failed to load JSON");

		assert(json.getType() == JSONValueType::Object && "JSON root is not an object");

		for (auto&& [key, value] : json)
		{
			assert(value.getType() == JSONValueType::Object && "Invalid JSON object");

			Jam::Domain::Enemy::EnemyStatus status;
			status.hp = value[U"hp"].get<int32>();
			status.attackPower = value[U"attackPower"].get<double>();
			status.moveSpeed = value[U"moveSpeed"].get<double>();
			status.jumpPower = value[U"jumpPower"].get<double>();
			status.colSize = { value[U"colX"].get<double>(), value[U"colY"].get<double>() };
			const auto& pm = value[U"physicsMaterial"];
			if (pm.getType() == JSONValueType::Object)
			{
				status.physicsMaterial = {
					pm[U"friction"].getOr<double>(0.2),
					pm[U"restitution"].getOr<double>(0.0),
					pm[U"density"].getOr<double>(1.0),
					pm[U"restitutionThreshold"].getOr<double>(1.0)
				};
			}

			using Jam::Domain::EnemyType;
			if (key == U"LittleDevil") outTable[EnemyType::LittleDevil] = status;
			else if (key == U"Ribbon") outTable[EnemyType::Ribbon] = status;
			else if (key == U"GothicLolitaDoll") outTable[EnemyType::GothicLolitaDoll] = status;
			else if (key == U"Spider") outTable[EnemyType::Spider] = status;
			else if (key == U"Eye") outTable[EnemyType::Eye] = status;
			else if (key == U"Clown") outTable[EnemyType::Clown] = status;
			else if (key == U"Boss1_3") outTable[EnemyType::Boss1_3] = status;
			else assert(false && "Unknown enemy type");
		}

		return true;
	}
}
