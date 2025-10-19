#pragma once
#include <Siv3D.hpp>
#include "../Domain/Enemy/EnemyBase.h"
#include "../Presentation/EnemyManager.h"
#include "../Domain/Physics/PhysicsTypes.h"
#include "../UseCase/EnemyFactory.h"
#include "../Infrastructure/FactoryServiceLocator.h"
#include "../Infrastructure/IPhysicsBodyFactory.h"


namespace Jam::Infrastructure
{
	class EnemyLoader
	{
	public:
		static bool loadEnemyForStageFromJSON(
			const FilePath& jsonPath,
			const std::unique_ptr<Jam::UseCase::EnemyFactory>& enemyFactory,
			const std::unique_ptr<Jam::Presentation::EnemyManager>& enemyManager,
			Jam::Domain::Physics::PhysicsBodyID playerId)
		{
			JSON json = JSON::Load(jsonPath);

			if (!json)
			{
				Console << U"[EnemyLoader] ❌ Failed to load stage enemy JSON: " << jsonPath;
				return false;
			}

			if (json.getType() != JSONValueType::Array)
			{
				Console << U"[EnemyLoader] ⚠ JSON root is not an array: " << jsonPath;
				return false;
			}

			for (size_t i = 0; i < json.size(); ++i)
			{
				const JSON& item = json[i];

				if (item.getType() != JSONValueType::Object)
				{
					Console << U"[EnemyLoader] ⚠ Invalid object at index: " << i;
					continue;
				}

				const String typeStr = item[U"type"].getString();
				const JSON& posJson = item[U"position"];
				const double posX = posJson[U"x"].get<double>();
				const double posY = posJson[U"y"].get<double>();

				Jam::UseCase::EnemyType type;
				if (typeStr == U"LittleDevil") type = Jam::UseCase::EnemyType::LittleDevil;
				else if (typeStr == U"Ribbon") type = Jam::UseCase::EnemyType::Ribbon;
				else
				{
					Console << U"[EnemyLoader] ⚠ Unknown enemy type: " << typeStr;
					continue;
				}

				const auto& statusTable = enemyFactory->getStatusTable();
				auto it = statusTable.find(type);
				if (it == statusTable.end())
				{
					Console << U"[EnemyLoader] ⚠ Enemy status not found for type: " << typeStr;
					continue;
				}

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

				auto enemy = enemyFactory->createEnemy(type, enemyBody, playerId);
				if (enemy)
				{
					enemyBody->setCollisionListener(enemy);
					int enemyId = enemyManager->AddEnemy(enemy, U"../Assets/Enemy/" + typeStr + U"/" + typeStr + U"_animation.json");
					enemyManager->getAnimator(enemyId).AddCondition({ { {U"isRunning", false} }, U"Idle", 0 });
					enemyManager->getAnimator(enemyId).SetBool(U"isRunning", false);
				}
				else
				{
					Console << U"[EnemyLoader] ❌ Failed to create enemy of type: " << typeStr;
				}
			}

			return true;
		}



		/// @brief JSON ファイルから敵ステータスを読み込む
		/// @param jsonPath 読み込む JSON ファイルのパス
		/// @param outTable 読み込んだステータスを格納するテーブル
		/// @return 成功したら true
		static bool LoadEnemyStatusFromJSON(
			const FilePath& jsonPath,
			std::unordered_map<Jam::UseCase::EnemyType, Jam::Domain::Enemy::EnemyStatus>& outTable)
		{
			JSON json = JSON::Load(jsonPath);

			if (not json)
			{
				Console << U"[EnemyLoader] ❌ Failed to load JSON: " << jsonPath;
				return false;
			}

			if (json.getType() != JSONValueType::Object)
			{
				Console << U"[EnemyLoader] ⚠ JSON root is not an object: " << jsonPath;
				return false;
			}

			for (auto&& [key, value] : json)
			{
				if (value.getType() != JSONValueType::Object)
				{
					Console << U"[EnemyLoader] ⚠ Invalid object for key: " << key;
					continue;
				}

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

				using Jam::UseCase::EnemyType;

				if (key == U"LittleDevil")
				{
					outTable[EnemyType::LittleDevil] = status;
				}
				else if (key == U"Ribbon")
				{	
					outTable[EnemyType::Ribbon] = status;
				}
				else
				{
					Console << U"[EnemyLoader] ⚠ Unknown enemy type: " << key;
				}
			}

			return true;
		}
	};
}
