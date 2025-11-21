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
		bool isSimple = false;//単色かどうか
		bool isRandomColor = true;
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
		ColorF color = Palette::Black;
		int32 fontSize = 32;
		double duration = 1.0;
		Vec2 velocity{ 0, -100 };
	};

	// リングエフェクトイベント（衝撃波など）
	struct RingEffectEvent
	{
		Vec2 position;
		ColorF color = Palette::Hotpink;
		double maxRadius = 150.0;
		double duration = 0.5;
		double thickness = 3.0;
	};

	struct FallDeathEffectEvent
	{
		Vec2 startPosition;           // 開始位置
		Vec2 knockbackDirection = Vec2::Up();      // 吹っ飛び方向
		double knockbackPower = 500.0; // 吹っ飛びの強さ
		double duration = 2.0;         // 演出の長さ
		double gravity = 980.0;        // 重力
		double rotationSpeed = 360.0;  // 回転速度（度/秒）
		ColorF starColor = Palette::Pink; // 星の色
		bool showExplosion = true;     // 最後に爆発を表示するか
	};

	// 破壊エフェクトイベント（バリア破壊）
	struct GlassShatterEffectEvent
	{
		Vec2 position;                // 破壊位置
		Vec2 impactDirection;         // 衝撃方向（正規化済み）
		ColorF glassColor = ColorF{0.5, 0.8, 1.0, 0.7};  // 水色透明
		int32 shardCount = 20;        // 破片数
		double shardSpeed = 300.0;    // 破片の飛散速度
		double duration = 1.0;        // 持続時間
		double barrierRadius = 150.0; // バリアの半径
	};

	struct ChokerSkillEffectEvent
	{
		Vec2 position;
		Vec2 direction;
	};

	using EffectEvent = std::variant<
		StarEffectEvent,
		ExplosionEffectEvent,
		ParticleEffectEvent,
		HitEffectEvent,
		TextEffectEvent,
		RingEffectEvent,
		FallDeathEffectEvent,
		GlassShatterEffectEvent,
		ChokerSkillEffectEvent
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
