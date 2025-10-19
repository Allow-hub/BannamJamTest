#pragma once
#include <HamFramework.hpp>
#include <variant>
#include <queue>
#include "../../UseCase/EnemyFactory.h"

namespace Jam::Domain::Events
{
	// Domain層のイベント定義
	struct EnemyDamagedEvent
	{
		Vec2 position;
		double damage;
		bool isCritical;
	};

	struct EnemyDefeatedEvent
	{
		Vec2 position;
		bool isBoss;
		Jam::UseCase::EnemyType enemyType;
	};

	struct PlayerAttackedEvent
	{
		Vec2 position;
		bool isHeavyAttack;
		double damage;
	};

	struct PlayerDamagedEvent
	{
		Vec2 position;
		double damage;
	};

	struct PlayerChokerSkillEvent
	{
		double zoom;
		double duration;
	};

	struct BossAppearedEvent
	{
		Vec2 position;
		Jam::UseCase::EnemyType bossName;
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
