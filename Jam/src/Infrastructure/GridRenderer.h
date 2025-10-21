#pragma once
#include <Siv3D.hpp>

namespace Jam::Util
{
	// グリッド表示クラス
	class GridRenderer
	{
	public:
		struct GridConfig
		{
			double gridSize = 100.0;                        // グリッドのサイズ（ピクセル）
			double gridRange = 10000.0;                     // グリッドの描画範囲（原点から±この値）
			ColorF lineColor{ 0.7, 0.7, 0.7, 0.5 };        // グリッド線の色
			ColorF axisColor{ 0.3, 0.3, 0.3, 0.8 };        // 軸の色
			ColorF textColor{ 0.0, 0.0, 0.0, 1.0 };        // 座標テキストの色（黒）
			ColorF textBackgroundColor{ 1.0, 1.0, 1.0, 0.8 }; // テキスト背景色（白）
			int fontSize = 14;                              // フォントサイズ
			bool showCoordinates = true;                    // 座標を表示するか
			bool showOriginAxes = true;                     // 原点の軸を表示するか
		};

		explicit GridRenderer(const GridConfig& config = GridConfig{})
			: m_config(config)
			, m_font(m_config.fontSize, Typeface::Bold)
		{
		}

		// シングルトン取得
		static GridRenderer& instance()
		{
			static GridRenderer s_instance;
			return s_instance;
		}

		// グリッドを描画（Vec2の0,0を原点として）
		void draw() const
		{
			const double gridSize = m_config.gridSize;
			const double range = m_config.gridRange;

			// 垂直線を描画
			for (double x = 0; x <= range; x += gridSize)
			{
				Line{ x, -range, x, range }.draw(m_config.lineColor);
				if (x > 0) // 負の方向にも
				{
					Line{ -x, -range, -x, range }.draw(m_config.lineColor);
				}
			}

			// 水平線を描画
			for (double y = 0; y <= range; y += gridSize)
			{
				Line{ -range, y, range, y }.draw(m_config.lineColor);
				if (y > 0) // 負の方向にも
				{
					Line{ -range, -y, range, -y }.draw(m_config.lineColor);
				}
			}

			// 原点の軸を強調表示
			if (m_config.showOriginAxes)
			{
				Line{ 0, -range, 0, range }.draw(3, m_config.axisColor); // Y軸
				Line{ -range, 0, range, 0 }.draw(3, m_config.axisColor); // X軸
			}

			// 座標ラベルを表示
			if (m_config.showCoordinates)
			{
				// X軸のラベル
				for (double x = 0; x <= range; x += gridSize)
				{
					if (x > 0)
					{
						drawTextWithBackground(U"{:.0f}"_fmt(x), Vec2{ x + 4, 4 });
						drawTextWithBackground(U"{:.0f}"_fmt(-x), Vec2{ -x + 4, 4 });
					}
				}

				// Y軸のラベル
				for (double y = gridSize; y <= range; y += gridSize)
				{
					drawTextWithBackground(U"{:.0f}"_fmt(y), Vec2{ 4, y + 4 });
					drawTextWithBackground(U"{:.0f}"_fmt(-y), Vec2{ 4, -y + 4 });
				}

				// 原点ラベル
				drawTextWithBackground(U"(0, 0)", Vec2{ 8, 8 }, ColorF{ 1.0, 0.0, 0.0 });
			}
		}

		// Rectの座標をデバッグ表示
		void drawRectDebug(const RectF& rect, const ColorF& color = Palette::Red) const
		{
			// Rectの枠線を描画
			rect.drawFrame(2, color);

			// 四隅に小さな円を描画
			Circle{ rect.tl(), 5 }.draw(color);
			Circle{ rect.tr(), 5 }.draw(color);
			Circle{ rect.bl(), 5 }.draw(color);
			Circle{ rect.br(), 5 }.draw(color);

			// 左上座標を表示
			const String coordText = U"({:.0f}, {:.0f})"_fmt(rect.x, rect.y);
			drawTextWithBackground(coordText, rect.tl() + Vec2{ 8, -24 }, color);

			// サイズを表示
			const String sizeText = U"{:.0f}x{:.0f}"_fmt(rect.w, rect.h);
			drawTextWithBackground(sizeText, rect.center(), color);
		}

		// Rect版
		void drawRectDebug(const Rect& rect, const ColorF& color = Palette::Red) const
		{
			drawRectDebug(RectF{ rect }, color);
		}

		// 任意の座標にラベルを表示
		void drawCoordinate(const Vec2& pos, const ColorF& color = Palette::Yellow) const
		{
			Circle{ pos, 8 }.draw(color);
			const String text = U"({:.0f}, {:.0f})"_fmt(pos.x, pos.y);
			drawTextWithBackground(text, pos + Vec2{ 12, -24 }, color);
		}

		void setConfig(const GridConfig& config)
		{
			m_config = config;
			m_font = Font(config.fontSize, Typeface::Bold);
		}

		const GridConfig& getConfig() const
		{
			return m_config;
		}

	private:
		// テキストを背景付きで描画
		void drawTextWithBackground(const String& text, const Vec2& pos,
									const ColorF& textColor = ColorF{}) const
		{
			const ColorF finalTextColor = (textColor.r == 0.0 && textColor.g == 0.0 && textColor.b == 0.0)
				? m_config.textColor : textColor;

			const auto region = m_font(text).region(pos);
			const RectF background = region.stretched(4, 2);

			// 背景を描画
			background.draw(m_config.textBackgroundColor);
			background.drawFrame(1, finalTextColor.withAlpha(0.5));

			// テキストを描画
			m_font(text).draw(pos, finalTextColor);
		}

		GridConfig m_config;
		Font m_font;
	};
}
