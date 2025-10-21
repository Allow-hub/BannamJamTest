#pragma once
#include <Siv3D.hpp>
#include <variant>
#include <queue>

namespace Jam::UseCase
{
	// 星エフェクトイベント
	struct StarEffectEvent
	{
		Vec2 position;
		double hue;
		int32 starCount = 6;
		double radius = 60.0;
		double duration = 0.4;
		double starSize = 36.0;
		Vec2 gravity{ 0, 160 };
		double hueVariation = 20.0;
	};

	// 爆発エフェクトイベント
	struct ExplosionEffectEvent
	{
		Vec2 position;
		ColorF color;
		double radius = 100.0;
		double duration = 0.6;
		int32 particleCount = 20;
	};

	// パーティクルエフェクトイベント
	struct ParticleEffectEvent
	{
		Vec2 position;
		Vec2 direction;
		ColorF color;
		int32 particleCount = 10;
		double speed = 100.0;
		double duration = 0.5;
	};

	// ヒットエフェクトイベント
	struct HitEffectEvent
	{
		Vec2 position;
		ColorF color = Palette::White;
		double scale = 1.0;
		double duration = 0.2;
	};

	// テキストエフェクトイベント（ダメージ表示など）
	struct TextEffectEvent
	{
		Vec2 position;
		String text;
		ColorF color = Palette::White;
		int32 fontSize = 24;
		double duration = 1.0;
		Vec2 velocity{ 0, -50 };
	};

	// リングエフェクトイベント（衝撃波など）
	struct RingEffectEvent
	{
		Vec2 position;
		ColorF color = Palette::White;
		double maxRadius = 150.0;
		double duration = 0.5;
		double thickness = 3.0;
	};

	using EffectEvent = std::variant<
		StarEffectEvent,
		ExplosionEffectEvent,
		ParticleEffectEvent,
		HitEffectEvent,
		TextEffectEvent,
		RingEffectEvent
	>;

	//エフェクトのキュー
	class EffectEventQueue
	{
	private:
		std::queue<EffectEvent> m_events;

	public:
		void push(const EffectEvent& event)
		{
			m_events.push(event);
		}

		bool empty() const
		{
			return m_events.empty();
		}

		EffectEvent pop()
		{
			auto event = m_events.front();
			m_events.pop();
			return event;
		}

		void clear()
		{
			while (!m_events.empty()) m_events.pop();
		}
	};
}
