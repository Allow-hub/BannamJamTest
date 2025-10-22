#pragma once
#include "EffectEvents.h"

namespace Jam::UseCase
{
	// 星エフェクト
	struct StarEffect : IEffect
	{
		struct Star
		{
			Vec2 start;
			Vec2 velocity;
			ColorF color;
		};

		Array<Star> m_stars;
		double m_duration;
		double m_starSize;
		Vec2 m_gravity;

		StarEffect(const StarEffectEvent& event)
			: m_duration(event.duration)
			, m_starSize(event.starSize)
			, m_gravity(event.gravity)
		{
			for (int32 i = 0; i < event.starCount; ++i)
			{
				const Vec2 velocity = RandomVec2(Circle{ event.radius });
				Star star{
					.start = (event.position + velocity * 0.5),
					.velocity = velocity,
					.color = HSV{ event.hue + Random(-event.hueVariation, event.hueVariation) },
				};
				m_stars << star;
			}
		}

		bool update(double t) override
		{
			t /= m_duration;
			for (auto& star : m_stars)
			{
				const Vec2 pos = (star.start + star.velocity * t + 0.5 * t * t * m_gravity);
				const double angle = (pos.x * 3_deg);
				const double size = m_starSize * (1.0 - t);
				Shape2D::Star(size, pos, angle).draw(star.color);
			}
			return (t < 1.0);
		}
	};

	// 爆発エフェクト
	struct ExplosionEffect : IEffect
	{
		struct Particle
		{
			Vec2 velocity;
			double size;
			ColorF color;
		};

		Vec2 m_center;
		Array<Particle> m_particles;
		double m_duration;

		ExplosionEffect(const ExplosionEffectEvent& event)
			: m_center(event.position)
			, m_duration(event.duration)
		{
			for (int32 i = 0; i < event.particleCount; ++i)
			{
				m_particles << Particle{
					.velocity = RandomVec2(Circle{ event.radius }),
					.size = Random(4.0, 12.0),
					.color = event.color
				};
			}
		}

		bool update(double t) override
		{
			t /= m_duration;
			for (const auto& particle : m_particles)
			{
				const Vec2 pos = m_center + particle.velocity * t;
				const double alpha = (1.0 - t);
				const double size = particle.size * (1.0 - t * 0.5);
				Circle{ pos, size }.draw(ColorF{ particle.color, alpha });
			}
			return (t < 1.0);
		}
	};

	// パーティクルエフェクト
	// パーティクルエフェクト
	struct ParticleEffect : IEffect
	{
		struct Particle
		{
			Vec2 start;
			Vec2 velocity;
			ColorF color;
		};

		Array<Particle> m_particles;
		double m_duration;

		ParticleEffect(const ParticleEffectEvent& event)
			: m_duration(event.duration)
		{
			// --- ランダム候補色 ---
			const Array<ColorF> colors = {
				Palette::Red,
				Palette::Orange,
				Palette::Yellow,
				Palette::Lime,
				Palette::Cyan,
				Palette::Skyblue,
				Palette::Pink,
				Palette::White
			};

			// --- パーティクル生成 ---
			for (int32 i = 0; i < event.particleCount; ++i)
			{
				const double angle = Random(0.0, Math::TwoPi);
				const Vec2 dir = event.direction.normalized();
				const Vec2 spread = Vec2{ Math::Cos(angle), Math::Sin(angle) } *Random(0.5, 1.0);

				ColorF particleColor;

				// --- カラー設定ロジック ---
				if (event.isSimple)
				{
					// 単色指定 → event.color をそのまま使用
					particleColor = event.color;
				}
				else if (event.isRandomColor)
				{
					// 各パーティクルごとにランダム
					particleColor = colors.choice();
				}
				else
				{
					// 全体で同じランダム色（1つ目で決める）
					static ColorF sharedColor = colors.choice();
					particleColor = sharedColor;
				}

				m_particles << Particle{
					.start = event.position,
					.velocity = (dir + spread * 0.3) * event.speed,
					.color = particleColor
				};
			}
		}

		bool update(double t) override
		{
			t /= m_duration;

			for (const auto& particle : m_particles)
			{
				const Vec2 pos = particle.start + particle.velocity * t;
				const double alpha = (1.0 - t);
				const double size = Random(4.0, 8.0);

				Circle{ pos, size }.draw(ColorF{ particle.color, alpha });
			}

			return (t < 1.0);
		}
	};



	// ヒットエフェクト
	struct HitEffect : IEffect
	{
		Vec2 m_position;
		ColorF m_color;
		double m_scale;
		double m_duration;

		HitEffect(const HitEffectEvent& event)
			: m_position(event.position)
			, m_color(event.color)
			, m_scale(event.scale)
			, m_duration(event.duration)
		{
		}

		bool update(double t) override
		{
			t /= m_duration;
			const double scale = m_scale * (1.0 + t * 0.5);
			const double alpha = (1.0 - t);
			Circle{ m_position, 20.0 * scale }.drawFrame(3.0, ColorF{ m_color, alpha });
			return (t < 1.0);
		}
	};

	// テキストエフェクト
	struct TextEffect : IEffect
	{
		Vec2 m_start;
		String m_text;
		ColorF m_color;
		Font m_font;
		double m_duration;
		Vec2 m_velocity;

		TextEffect(const TextEffectEvent& event)
			: m_start(event.position)
			, m_text(event.text)
			, m_color(event.color)
			, m_font(event.fontSize, Typeface::Bold)
			, m_duration(event.duration)
			, m_velocity(event.velocity)
		{
		}

		bool update(double t) override
		{
			t /= m_duration;
			const Vec2 pos = m_start + m_velocity * t;
			const double alpha = (1.0 - t);
			m_font(m_text).drawAt(pos, ColorF{ m_color, alpha });
			return (t < 1.0);
		}
	};

	// リングエフェクト
	struct RingEffect : IEffect
	{
		Vec2 m_center;
		ColorF m_color;
		double m_maxRadius;
		double m_duration;
		double m_thickness;

		RingEffect(const RingEffectEvent& event)
			: m_center(event.position)
			, m_color(event.color)
			, m_maxRadius(event.maxRadius)
			, m_duration(event.duration)
			, m_thickness(event.thickness)
		{
		}

		bool update(double t) override
		{
			t /= m_duration;
			const double radius = m_maxRadius * t;
			const double alpha = (1.0 - t);
			Circle{ m_center, radius }.drawFrame(m_thickness, ColorF{ m_color, alpha });
			return (t < 1.0);
		}
	};
}
