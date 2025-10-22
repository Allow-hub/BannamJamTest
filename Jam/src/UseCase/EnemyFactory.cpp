#include "EnemyFactory.h"
#include "../Domain/Enemy/LittleDevil.h"
#include "../Domain/Enemy/Ribbon.h"

namespace Jam::UseCase
{
	using namespace Jam::Domain::Enemy;

	std::shared_ptr<EnemyBase> EnemyFactory::createEnemy(
		EnemyType type,
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
		Jam::Domain::Physics::PhysicsBodyID playerId) const
	{
		std::shared_ptr<EnemyBase> enemy = nullptr;

		switch (type)
		{
		case EnemyType::LittleDevil:
			enemy = std::make_shared<LittleDevil>(body, playerId);
			break;

		case EnemyType::Ribbon:
			// TODO: Ribbonの実装
			enemy = std::make_shared<Ribbon>(body, playerId);
			break;

		default:
			Print << U"[EnemyFactory] ⚠ Unknown enemy type!";
			return nullptr;
		}

		if (!enemy)
		{
			//Print << U"[EnemyFactory] ❌ Failed to create enemy instance";
			return nullptr;
		}

		auto enemyBody = enemy->getPhysicsBody();
		//Print << U"[EnemyFactory] Enemy Body ptr == body ptr ? " << (enemyBody.get() == body.get());

		// ステータステーブルに登録があれば適用
		auto it = m_statusTable.find(type);
		if (it != m_statusTable.end())
		{
			enemy->setStatus(it->second);
			//Print << U"[EnemyFactory] ✅ Applied status: HP=" << it->second.hp
				//<< U", Speed=" << it->second.moveSpeed;
		}
		return enemy;
	}
}
