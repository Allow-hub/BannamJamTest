#pragma once
#include <HamFramework.hpp>
#include <variant>
#include <queue>
#include <memory>
#include "../Physics/PhysicsBodyID.h"
#include "../ITakeDamageable.h"
#include "../Enemy/EnemyType.h"

// インクルードすると、循環してしまうため前方宣言
namespace Jam::Domain::Enemy
{
	class EnemyBase;
}

namespace Jam::Domain::Events
{
	// Domain層のイベント定義
	struct EnemyDamagedEvent
	{
		Jam::Domain::Physics::PhysicsBodyID attacker;
		Jam::Domain::Physics::PhysicsBodyID target;
		Jam::Domain::DamageInfo damageInfo;
	};

	struct EnemyDefeatedEvent
	{
		Vec2 position;
		bool isBoss;
		Jam::Domain::EnemyType enemyType;
		double zoom;
		double duration;
		double intensity;
	};

	struct PlayerAttackedEvent
	{
		double zoom;
		double duration;
		double intensity;
	};

	struct PlayerDeathEvent
	{
		double zoom;
		double duration;
		double intensity;
	};

	struct PlayerFallOutEvent
	{
		double zoom;
		double duration;
		double intensity;
		Vec2 pos;
	};

	struct PlayerDamagedEvent
	{
		Jam::Domain::Physics::PhysicsBodyID attacker;
		Jam::Domain::Physics::PhysicsBodyID target;
		Jam::Domain::DamageInfo damageInfo;
		double zoom;
		double duration;
		double intensity;
	};

	struct PlayerChokerSkillEvent
	{
		double zoom;
		double duration;
	};

	struct BossAppearedEvent
	{
		Vec2 position;
		Jam::Domain::EnemyType bossName;
	};

	struct ItemCollectedEvent
	{
		Vec2 position;
		bool isRare;
		String itemName;
	};

	struct EnemySpawnedEvent
	{
		std::shared_ptr<Jam::Domain::Enemy::EnemyBase> enemy;
		s3d::FilePath animationPath; // 今後別の敵のアニメーションのパスを指定する場合は、EnemyTypeから適切なパスを返すヘルパー関数を作成すること
	};
	struct ExplosionEvent
	{
		Vec2 position;
		ColorF color;
		double radius = 100.0;
		double duration = 0.6;
		int32 particleCount = 20;
	};

	using GameEvent = std::variant<
		EnemyDamagedEvent,
		EnemyDefeatedEvent,
		PlayerAttackedEvent,
		PlayerChokerSkillEvent,
		PlayerDamagedEvent,
		PlayerDeathEvent,
		PlayerFallOutEvent,
		BossAppearedEvent,
		ItemCollectedEvent,
		EnemySpawnedEvent,
		ExplosionEvent
	>;


	//　ゲーム内でドメイン層が起こすイベントのQueue
	class GameEventQueue
	{
	private:
		std::queue<GameEvent> m_events;

	public:
		void push(const GameEvent& event)
		{
			m_events.push(event);
		}

		bool empty() const
		{
			return m_events.empty();
		}

		GameEvent pop()
		{
			auto event = m_events.front();
			m_events.pop();
			return event;
		}

		void clear()
		{
			while (!m_events.empty()) m_events.pop();
		}

		size_t size() const
		{
			return m_events.size();
		}
	};
}
