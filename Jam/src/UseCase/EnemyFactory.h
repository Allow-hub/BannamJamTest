#pragma once
#include <memory>
#include <unordered_map>
#include "../Domain/Enemy/EnemyBase.h"
#include "../Domain/Physics/IPhysicsBody.h"

namespace Jam::UseCase
{
	enum class EnemyType
	{
		LittleDevil,
		Ribbon,
	};

	class EnemyFactory
	{
	public:
		EnemyFactory() = default;

		/// ステータスデータを登録
		static	void SetStatusTable(const std::unordered_map<EnemyType, Jam::Domain::Enemy::EnemyStatus>& table)
		{
			m_statusTable = table;
		}

		///  敵を生成
		static std::shared_ptr<Jam::Domain::Enemy::EnemyBase> CreateEnemy(
			EnemyType type,
			std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body);

	private:
		static	std::unordered_map<EnemyType, Jam::Domain::Enemy::EnemyStatus> m_statusTable;
	};
}
