#pragma once
#include <HamFramework.hpp>
#include <variant>
#include <queue>
#include "../Physics/PhysicsBodyID.h"
#include "../ITakeDamageable.h"
#include "../Enemy/EnemyType.h"

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

	using GameEvent = std::variant<
		EnemyDamagedEvent,
		EnemyDefeatedEvent,
		PlayerAttackedEvent,
		PlayerChokerSkillEvent,
		PlayerDamagedEvent,
		BossAppearedEvent,
		ItemCollectedEvent
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
