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
		double m_radius;
		ColorF m_baseColor;

		ExplosionEffect(const ExplosionEffectEvent& event)
			: m_center(event.position)
			, m_duration(event.duration)
			, m_radius(event.radius)
			, m_baseColor(event.color)
		{
			for (int32 i = 0; i < event.particleCount; ++i)
			{
				// 色を少しずらす（赤ならオレンジ混ぜるなど）
				double hueShift = Random(-0.5, 0.5);
				ColorF particleColor = m_baseColor;
				particleColor.r = Math::Clamp(particleColor.r + hueShift, 0.0, 1.0);
				particleColor.g = Math::Clamp(particleColor.g + hueShift * 0.5, 0.0, 1.0);
				particleColor.b = Math::Clamp(particleColor.b, 0.0, 1.0);

				m_particles << Particle{
					.velocity = RandomVec2(Circle{ m_radius }),
					.size = Random(4.0, 12.0),
					.color = particleColor
				};
			}
		}

		bool update(double t) override
		{
			t /= m_duration;

			// 爆心の芯サークル
			double coreSize = 0.0;
			if (t < 0.5)
			{
				// 0 → 半径
				coreSize = m_radius * (t / 0.5);
			}
			else
			{
				// 半径 → 0
				coreSize = m_radius * ((1.0 - t) / 0.5);
			}
			Circle{ m_center, coreSize }.draw(ColorF{ m_baseColor, 0.8 * (1.0 - t) });

			// 粒子描画
			for (const auto& particle : m_particles)
			{
				Vec2 pos = m_center + particle.velocity * t;
				double alpha = (1.0 - t);
				double size = particle.size * (1.0 - t * 0.5);
				Circle{ pos, size }.draw(ColorF{ particle.color, alpha });
			}

			return t < 1.0;
		}
	};


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

			// 全体で使う色を事前に決定（staticを使わない）
			const ColorF sharedColor = colors.choice();

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
					// 全体で同じランダム色
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
	// 落下演出エフェクト
	// バージョン1: 光の柱と共に消える演出
	struct FallDeathEffect : IEffect
	{
		Vec2 m_startPos;
		Vec2 m_velocity;
		double m_duration;
		double m_gravity;
		ColorF m_mainColor;
		bool m_explosionTriggered = false;
		std::function<void(const ExplosionEffectEvent&)> m_onExplosion;

		FallDeathEffect(const FallDeathEffectEvent& event,
						std::function<void(const ExplosionEffectEvent&)> onExplosion = nullptr)
			: m_startPos(event.startPosition)
			, m_velocity(event.knockbackDirection.normalized()* event.knockbackPower)
			, m_duration(event.duration)
			, m_gravity(event.gravity)
			, m_mainColor(event.starColor)
			, m_onExplosion(onExplosion)
		{
		}

		bool update(double t) override
		{
			const double normalizedTime = t / m_duration;

			// 物理演算
			const Vec2 gravityVec = Vec2{ 0, m_gravity };
			const Vec2 currentPos = m_startPos + m_velocity * t + 0.5 * gravityVec * t * t;

			// フェーズ1: 吹っ飛び (0.0 - 0.6)
			if (normalizedTime < 0.6)
			{
				const double phase1 = normalizedTime / 0.6;
				const double size = Math::Lerp(40.0, 25.0, phase1);
				const double rotation = t * 720.0 * 1_deg;

				// メインの星
				Shape2D::Star(size, currentPos, rotation)
					.draw(m_mainColor);

				// 光の軌跡
				for (int i = 1; i <= 5; ++i)
				{
					const double trailTime = t - i * 0.03;
					if (trailTime > 0)
					{
						const Vec2 trailPos = m_startPos + m_velocity * trailTime +
							0.5 * gravityVec * trailTime * trailTime;
						const double trailAlpha = 0.6 * (1.0 - i * 0.15);
						const double trailSize = size * (1.0 - i * 0.15);

						Circle{ trailPos, trailSize * 0.5 }
						.draw(ColorF{ m_mainColor, trailAlpha });
					}
				}

				// 周囲のパーティクル
				if (Random(1.0) < 0.3)
				{
					const Vec2 offset = RandomVec2(Circle{ 20.0 });
					Circle{ currentPos + offset, Random(2.0, 5.0) }
					.draw(ColorF{ m_mainColor, 0.8 });
				}
			}
			// フェーズ2: 光の柱出現 (0.6 - 0.85)
			else if (normalizedTime < 0.85)
			{
				const double phase2 = (normalizedTime - 0.6) / 0.25;

				// 縮小していく星
				const double size = Math::Lerp(25.0, 15.0, phase2);
				const double starAlpha = 1.0 - phase2 * 0.5;
				Shape2D::Star(size, currentPos, t * 1080.0 * 1_deg)
					.draw(ColorF{ m_mainColor, starAlpha });

				// 光の柱（下から上へ）
				const double beamHeight = Math::Lerp(-100.0, 300.0, EaseOutExpo(phase2));
				const double beamWidth = 80.0 * (1.0 - phase2 * 0.3);
				const double beamAlpha = Math::Lerp(0.8, 0.3, phase2);

				RectF{ currentPos.x - beamWidth / 2, currentPos.y, beamWidth, beamHeight }
				.draw(ColorF{ m_mainColor, beamAlpha * 0.3 });
				RectF{ currentPos.x - beamWidth / 2, currentPos.y, beamWidth, beamHeight }
				.drawFrame(3.0, ColorF{ m_mainColor, beamAlpha });

				// 光の輪
				for (int i = 0; i < 3; ++i)
				{
					const double ringRadius = 50.0 + i * 25.0 + phase2 * 50.0;
					const double ringAlpha = beamAlpha * (1.0 - i * 0.3);
					Circle{ currentPos, ringRadius }
					.drawFrame(2.0, ColorF{ m_mainColor, ringAlpha * 0.5 });
				}
			}
			// フェーズ3: 爆発と消滅 (0.85 - 1.0)
			else
			{
				const double phase3 = (normalizedTime - 0.85) / 0.15;

				if (!m_explosionTriggered && m_onExplosion)
				{
					// 爆発エフェクト
					m_onExplosion(ExplosionEffectEvent{
						.position = currentPos,
						.color = m_mainColor,
						.radius = 120.0,
						.duration = 0.5,
						.particleCount = 40
					});

					// リングエフェクト
					m_onExplosion(ExplosionEffectEvent{
						.position = currentPos,
						.color = ColorF{ 1.0, 1.0, 1.0 },
						.radius = 80.0,
						.duration = 0.3,
						.particleCount = 20
					});

					m_explosionTriggered = true;
				}

				// フェードアウトする光
				const double fadeAlpha = 1.0 - phase3;
				Circle{ currentPos, 80.0 * (1.0 + phase3) }
				.draw(ColorF{ m_mainColor, fadeAlpha * 0.3 });
			}

			return (normalizedTime < 1.0);
		}
	};

	// 破壊エフェクト（バリア破壊）
	struct GlassShatterEffect : IEffect
	{
		struct Shard
		{
			Vec2 position;
			Vec2 velocity;
			double rotation;
			double rotationSpeed;
			double size;
			ColorF color;
		};

		Vec2 m_center;
		Vec2 m_impactDir;
		Array<Shard> m_shards;
		double m_duration;
		double m_gravity;
		ColorF m_glassColor;

		GlassShatterEffect(const GlassShatterEffectEvent& event)
			: m_center(event.position)
			, m_impactDir(event.impactDirection.normalized())
			, m_duration(event.duration)
			, m_gravity(500.0)
			, m_glassColor(event.glassColor)
		{
			// 破片を生成
			for (int32 i = 0; i < event.shardCount; ++i)
			{
				// 衝撃方向を中心に扇状に飛散
				const double spreadAngle = Random(-60.0, 60.0) * 1_deg;
				const double baseAngle = Math::Atan2(m_impactDir.y, m_impactDir.x);
				const double shardAngle = baseAngle + spreadAngle;
				
				const Vec2 direction = Vec2{
					Math::Cos(shardAngle),
					Math::Sin(shardAngle)
				};
				
				// 衝突方向に基づいて横方向にオフセット（左から当たったら左から出る）
				const double sideOffset = 120.0 * 1_deg;
				const double startAngle = baseAngle + sideOffset; // 横方向
				const Vec2 startDirection = Vec2{
					Math::Cos(startAngle),
					Math::Sin(startAngle)
				};
				const double distFromCenter = event.barrierRadius * Random(0.8, 1.0);
				const Vec2 startPos = m_center + startDirection * distFromCenter;
				
				// 速度（衝撃方向に強く飛ぶ）
				const double speedVariation = Random(0.7, 1.3);
				const Vec2 velocity = direction * event.shardSpeed * speedVariation;
				
				// 色のバリエーション
				ColorF shardColor = m_glassColor;
				shardColor.r += Random(-0.1, 0.1);
				shardColor.g += Random(-0.1, 0.1);
				shardColor.b += Random(-0.1, 0.1);
				shardColor.a = m_glassColor.a;
				
				m_shards << Shard{
					.position = startPos,
					.velocity = velocity,
					.rotation = Random(0.0, Math::TwoPi),
					.rotationSpeed = Random(-720.0, 720.0) * 1_deg,
					.size = Random(20.0, 40.0),
					.color = shardColor
				};
			}
		}

		bool update(double t) override
		{
			const double normalizedTime = t / m_duration;
			const Vec2 gravityVec = Vec2{ 0, m_gravity };
			
			// フェーズ1: 初期衝撃（0.0 - 0.2）
			if (normalizedTime < 0.2)
			{
				const double phase1 = normalizedTime / 0.2;
				
				// 衝撃波リング
				const double ringRadius = 150.0 * phase1;
				const double ringAlpha = (1.0 - phase1) * 0.8;
				Circle{ m_center, ringRadius }
					.drawFrame(3.0, ColorF{ m_glassColor, ringAlpha });
				
				// フラッシュ
				const double flashAlpha = (1.0 - phase1) * 0.5;
				Circle{ m_center, 100.0 }
					.draw(ColorF{ 1.0, 1.0, 1.0, flashAlpha });
			}
			
			// 破片描画
			for (auto& shard : m_shards)
			{
				// 物理演算
				const Vec2 pos = shard.position + shard.velocity * t + 0.5 * gravityVec * t * t;
				const double rotation = shard.rotation + shard.rotationSpeed * t;
				
				// フェードアウト
				const double alpha = m_glassColor.a * (1.0 - normalizedTime * 0.7);
				
				// 破片の形状（四角形を回転）
				const double size = shard.size * (1.0 - normalizedTime * 0.3);
				const RectF shardRect{ Arg::center = pos, size, size };
				
				// 輪郭と塗りつぶし
				shardRect.rotated(rotation).draw(ColorF{ shard.color, alpha * 0.6 });
				shardRect.rotated(rotation).drawFrame(1.5, ColorF{ 1.0, 1.0, 1.0, alpha });
				
				// キラキラ感
				if (Random(1.0) < 0.1)
				{
					Circle{ pos, Random(2.0, 4.0) }
						.draw(ColorF{ 1.0, 1.0, 1.0, alpha });
				}
			}
			
			return (normalizedTime < 1.0);
		}
	};
}
