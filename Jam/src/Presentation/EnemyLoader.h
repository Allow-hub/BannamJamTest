#pragma once
#include <Siv3D.hpp>
#include "../Domain/Enemy/EnemyBase.h"
#include "../Domain/Physics/PhysicsTypes.h"
#include "../UseCase/EnemyFactory.h"

namespace Jam::Presentation
{
	class EnemyLoader
	{
	public:

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
