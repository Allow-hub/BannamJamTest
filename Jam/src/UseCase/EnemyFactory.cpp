#include "UseCase/EnemyFactory.h"
#include "Domain/Enemy/LittleDevil.h"
#include "UseCase/AttackProcessor.h"
#include "Domain/Events/GameEvents.h"
#include "Domain/Enemy/Ribbon.h"
#include "Domain/Enemy/GothicLolitaDoll.h"
#include "Domain/Enemy/Spider.h"
#include "Domain/Enemy/Eye.h"
#include "Domain/Enemy/Clown.h"
#include "Domain/Enemy/Boss1_3.h"
#include "Domain/Enemy/ToxicPlant.h"
#include "Domain/Enemy/ParalysisPlant.h"

namespace Jam::UseCase
{
	using namespace Jam::Domain::Enemy;

	std::shared_ptr<EnemyBase> EnemyFactory::createEnemy(
		Jam::Domain::EnemyType type,
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
		Jam::Domain::Physics::PhysicsBodyID playerId,
		Jam::Domain::Events::GameEventQueue& eventQueue) const
	{
		std::shared_ptr<EnemyBase> enemy = nullptr;

		switch (type)
		{
		case Jam::Domain::EnemyType::LittleDevil:
			enemy = std::make_shared<LittleDevil>(body, playerId, eventQueue);
			break;
		case Jam::Domain::EnemyType::Ribbon:
			enemy = std::make_shared<Ribbon>(body, playerId, eventQueue);
			break;
		case Jam::Domain::EnemyType::GothicLolitaDoll:
			enemy = std::make_shared<GothicLolitaDoll>(body, playerId, eventQueue);
			break;
		case Jam::Domain::EnemyType::Spider:
			enemy = std::make_shared<Spider>(body, playerId, eventQueue);
			break;
		case Jam::Domain::EnemyType::Eye:
			enemy = std::make_shared<Eye>(body, playerId, eventQueue);
			break;
		case Jam::Domain::EnemyType::Clown:
			enemy = std::make_shared<Clown>(body, playerId, eventQueue);
			break;
		case Jam::Domain::EnemyType::ToxicPlant:
			enemy = std::make_shared<ToxicPlant>(body, playerId, eventQueue);
			break;
		case Jam::Domain::EnemyType::ParalysisPlant:
			enemy = std::make_shared<ParalysisPlant>(body, playerId, eventQueue);
			break;
		case Jam::Domain::EnemyType::Boss1_3:
			enemy = std::make_shared<Boss1_3>(body, playerId, eventQueue);
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

		// ステータステーブルに登録があれば適用
		auto it = m_statusTable.find(type);
		if (it != m_statusTable.end())
		{
			enemy->setStatus(it->second);
			//Print << U"[EnemyFactory] ✅ Applied status: HP=" << it->second.hp
			//	<< U", Speed=" << it->second.moveSpeed;
		}

		//攻撃対象に追加
		Jam::UseCase::AttackProcessor::getInstance().registerDamageable(enemyBody->getID(), enemy);
		return enemy;
	}
}
