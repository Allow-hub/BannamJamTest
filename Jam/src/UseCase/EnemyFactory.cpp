#include "EnemyFactory.h"
#include "../Domain/Enemy/LittleDevil.h"

namespace Jam::UseCase
{
	using namespace Jam::Domain::Enemy;

	 std::shared_ptr<EnemyBase> EnemyFactory::CreateEnemy(
		EnemyType type,
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body)
	{
		std::shared_ptr<EnemyBase> enemy = nullptr;

		switch (type)
		{
		case EnemyType::LittleDevil:
			enemy = std::make_shared<LittleDevil>(std::move(body));
			break;

		default:
			Console << U"[EnemyFactory] ⚠ Unknown enemy type!";
			return nullptr;
		}
		auto enemyBody = enemy->getPhysicsBody();
		Console << U"Enemy Body ptr == body ptr ? " << (enemyBody.get() == body.get());
		// ステータステーブルに登録があれば適用
		if (auto it = m_statusTable.find(type); it != m_statusTable.end())
		{
			enemy->setStatus(it->second);
		}
		else
		{
			Console << U"[EnemyFactory] ⚠ No status found for enemy type!";
		}

		return enemy;
	}
	 std::unordered_map<EnemyType, Jam::Domain::Enemy::EnemyStatus> EnemyFactory::m_statusTable;
}
