#pragma once
#include <memory>
#include <unordered_map>
#include "../Domain/Enemy/EnemyBase.h"
#include "../Domain/Physics/IPhysicsBody.h"
#include "../Domain/Physics/PhysicsBodyID.h"

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

		explicit EnemyFactory(
			const std::unordered_map<EnemyType, Jam::Domain::Enemy::EnemyStatus>& statusTable)
			: m_statusTable(statusTable)
		{
		}

		void setStatusTable(
			const std::unordered_map<EnemyType, Jam::Domain::Enemy::EnemyStatus>& table)
		{
			m_statusTable = table;
		}

		std::shared_ptr<Jam::Domain::Enemy::EnemyBase> createEnemy(
			EnemyType type,
			std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId) const;

	private:
		std::unordered_map<EnemyType, Jam::Domain::Enemy::EnemyStatus> m_statusTable;
	};
}
