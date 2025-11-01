#pragma once
#include <Siv3D.hpp>

namespace Jam::Presentation
{
	class BloomManager
	{
	private:
		struct BloomLayer
		{
			RenderTexture blur;
			RenderTexture internal;
			int32 downsampleFactor;
			double intensity;
			BloomLayer(const Size& baseSize, int32 factor, double defaultIntensity = 0.5)
				: blur(baseSize / factor)
				, internal(baseSize / factor)
				, downsampleFactor(factor)
				, intensity(defaultIntensity)
			{
			}
		};
		Array<BloomLayer> layers;
		ColorF clearColor;
		Size baseSize;

		// Vignette設定
		double vignetteIntensity = 0.5;
		double vignetteSmoothness = 0.5;
		ColorF vignetteColor = ColorF{ 0.0, 0.0, 0.0 };

		BloomManager(const Size& sceneSize)
			: clearColor(0.0)
			, baseSize(sceneSize)
		{
			// ブルームレイヤーを初期化（1x, 1/4x, 1/8x, 1/16x）
			layers.emplace_back(sceneSize, 1, 0.5);   // フル解像度
			layers.emplace_back(sceneSize, 4, 0.5);   // 1/4
			layers.emplace_back(sceneSize, 8, 0.5);   // 1/8
			layers.emplace_back(sceneSize, 16, 0.5);  // 1/16
			layers[0].blur.clear(clearColor);
		}

		// Vignetteを描画する内部関数
		void drawVignetteEffect() const
		{
			if (vignetteIntensity <= 0.0) return;

			const Size sceneSize = Scene::Size();
			const Vec2 center = Scene::Center();

			// 横長・縦長を考慮した楕円の最大半径
			const double maxRadiusX = sceneSize.x * 1.2;
			const double maxRadiusY = sceneSize.y * 1.2;

			// ステップ数（滑らかさ）
			const int steps = 60;

			const ScopedRenderStates2D blend{ BlendState::Default2D };

			for (int i = 0; i < steps; ++i)
			{
				const double t = static_cast<double>(i) / steps; // 0〜1
				const double radiusX = maxRadiusX * (1.0 - t * vignetteSmoothness);
				const double radiusY = maxRadiusY * (1.0 - t * vignetteSmoothness);

				// 中心から離れるほど黒くなる
				const double alpha = vignetteIntensity * (t * t); // tが大きくなるほどα増加

				Ellipse{ center, radiusX, radiusY }.drawFrame(
					Max(maxRadiusX, maxRadiusY) / steps, 0, vignetteColor.withAlpha(alpha)
				);
			}

			// 内側は透明（中心は描画せず）
			const double innerX = maxRadiusX * (1.0 - vignetteSmoothness);
			const double innerY = maxRadiusY * (1.0 - vignetteSmoothness);

			// 中心部分を透明で塗りつぶす
			Ellipse{ center, innerX, innerY }.draw(ColorF{ 0.0, 0.0, 0.0, 0.0 });
		}

	public:
		static BloomManager& getInstance()
		{
			static BloomManager instance(Scene::Size());
			return instance;
		}

		[[nodiscard]]
		ScopedRenderTarget2D getRenderTarget()
		{
			return ScopedRenderTarget2D(layers[0].blur);
		}

		// Vignette設定
		void setVignetteIntensity(double intensity)
		{
			vignetteIntensity = Math::Clamp(intensity, 0.0, 1.0);
		}

		void setVignetteSmoothness(double smoothness)
		{
			vignetteSmoothness = Math::Clamp(smoothness, 0.0, 1.0);
		}

		void setVignetteColor(const ColorF& color)
		{
			vignetteColor = color;
		}

		void setVignette(double intensity, double smoothness, const ColorF& color = ColorF{ 0.0 })
		{
			setVignetteIntensity(intensity);
			setVignetteSmoothness(smoothness);
			setVignetteColor(color);
		}

		double getVignetteIntensity() const { return vignetteIntensity; }
		double getVignetteSmoothness() const { return vignetteSmoothness; }
		ColorF getVignetteColor() const { return vignetteColor; }

		// すべてのレイヤー強度を一括設定
		void setIntensities(double a1, double a4, double a8, double a16)
		{
			if (layers.size() >= 4)
			{
				layers[0].intensity = a1;
				layers[1].intensity = a4;
				layers[2].intensity = a8;
				layers[3].intensity = a16;
			}
		}

		// 全体のブルーム強度を調整
		void setGlobalIntensity(double intensity)
		{
			for (auto& layer : layers)
			{
				layer.intensity *= intensity;
			}
		}

		// デフォルト強度で描画（Vignette込み）
		void draw(bool applyVignette = true)
		{
			drawWithIntensities(layers[0].intensity,
							   layers[1].intensity,
							   layers[2].intensity,
							   layers[3].intensity,
							   applyVignette);
		}

		// カスタム強度で描画（4段階）
		void drawWithIntensities(double a1, double a4, double a8, double a16, bool applyVignette = true)
		{
			// 元画像を描画
			layers[0].blur.resized(Scene::Size()).draw();

			// 各レイヤーにガウシアンブラーを適用
			for (size_t i = 0; i < layers.size(); ++i)
			{
				if (i == 0)
				{
					// 最初のレイヤー
					Shader::GaussianBlur(layers[0].blur, layers[0].internal, layers[0].blur);
				}
				else
				{
					// 前のレイヤーからダウンサンプリング
					Shader::Downsample(layers[i - 1].blur, layers[i].blur);
					Shader::GaussianBlur(layers[i].blur, layers[i].internal, layers[i].blur);
				}
			}

			// 加算合成でブルームを描画
			{
				const ScopedRenderStates2D blend{ BlendState::Additive };

				if (a1 > 0.0)
				{
					layers[0].blur.resized(Scene::Size()).draw(ColorF{ a1 });
				}
				if (a4 > 0.0)
				{
					layers[1].blur.resized(Scene::Size()).draw(ColorF{ a4 });
				}
				if (a8 > 0.0)
				{
					layers[2].blur.resized(Scene::Size()).draw(ColorF{ a8 });
				}
				if (a16 > 0.0)
				{
					layers[3].blur.resized(Scene::Size()).draw(ColorF{ a16 });
				}
			}

			// Vignetteエフェクトを適用
	/*		if (applyVignette)
			{
				drawVignetteEffect();
			}*/

			// 次のフレームのためにクリア
			layers[0].blur.clear(clearColor);
		}

		// 可変引数版（任意の段階数に対応）
		void drawWithIntensities(const Array<double>& intensities, bool applyVignette = true)
		{
			// 元画像を描画
			layers[0].blur.resized(Scene::Size()).draw();

			// 各レイヤーにガウシアンブラーを適用
			for (size_t i = 0; i < layers.size(); ++i)
			{
				if (i == 0)
				{
					Shader::GaussianBlur(layers[0].blur, layers[0].internal, layers[0].blur);
				}
				else
				{
					Shader::Downsample(layers[i - 1].blur, layers[i].blur);
					Shader::GaussianBlur(layers[i].blur, layers[i].internal, layers[i].blur);
				}
			}

			// 加算合成でブルームを描画
			{
				const ScopedRenderStates2D blend{ BlendState::Additive };

				const size_t count = (layers.size() < intensities.size()) ? layers.size() : intensities.size();
				for (size_t i = 0; i < count; ++i)
				{
					if (intensities[i] > 0.0)
					{
						layers[i].blur.resized(Scene::Size()).draw(ColorF{ intensities[i] });
					}
				}
			}

			// Vignetteエフェクトを適用
			if (applyVignette)
			{
				drawVignetteEffect();
			}

			layers[0].blur.clear(clearColor);
		}

		// Vignetteのみを描画（Bloomなし）
		void drawVignetteOnly()
		{
			drawVignetteEffect();
		}
	};
}
