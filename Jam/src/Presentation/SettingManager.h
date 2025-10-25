#pragma once
#include <Siv3D.hpp>

namespace Jam::Presentation
{
	class SettingManager
	{
	private:
		SettingManager() = default;
		Font font;
		Texture m_backgroundTexture;

		struct Button
		{
			String label;
			Vec2 pos;
			int size;
			RectF rect;

			Button(const String& l, const Vec2& p, int s)
				: label(l), pos(p), size(s)
			{
				// 初期化時は幅を仮置き。draw時に再計算する
				rect = RectF(pos, Vec2(200, s));
			}

			bool isHovered() const { return rect.mouseOver(); }

			void draw(const Font& font)
			{
				// 描画時に幅を計算して rect を更新
				const double w = font(label).region().w * (size / 40.0);
				rect = RectF(pos, Vec2(w, size));

				const ColorF col = isHovered() ? Palette::Yellow : Palette::White;
				font(label).draw(size, pos, col);
			}
		};


		Array<Button> buttons;

	public:
		static SettingManager& Instance()
		{
			static SettingManager instance;
			return instance;
		}

		void init()
		{
			m_backgroundTexture = Texture{ U"../Assets/Stage/BG.png" };
			const int32 mainFontSize = static_cast<int32>(Scene::Height() * 0.07);
			font = Font(mainFontSize, U"../Assets/Font/PixelMplus12-Bold.ttf", FontStyle::Bold);

			// ボタンを作成
			buttons.emplace_back(U"AUDIO", Vec2(70, 120), 60);
			buttons.emplace_back(U"RETRY", Vec2(70, 220), 60);
			buttons.emplace_back(U"EXIT", Vec2(70, 320), 60);
			buttons.emplace_back(U"BACK THE SELECT", Vec2(70, 420), 60);
		}

		void update()
		{
			// ボタンクリック判定
			for (auto& b : buttons)
			{
				if (b.isHovered() && MouseL.down())
				{
					// ボタンごとの処理
					if (b.label == U"AUDIO")
					{
						Print << U"Audio button clicked";
					}
					else if (b.label == U"RETRY")
					{
						Print << U"Retry button clicked";
					}
					else if (b.label == U"EXIT")
					{
						Print << U"Exit button clicked";
					}
					else if (b.label == U"BACK THE SELECT")
					{
						Print << U"Back button clicked";
					}
				}
			}
		}

		void draw()
		{
			// 背景描画
			m_backgroundTexture.resized(Scene::Size()).draw(0, 0);
			Rect{ 0, 0, Scene::Size().x, Scene::Size().y }.draw(ColorF{ 1.0, 1.0, 1.0 ,0.3 });

			// 左上タイトル
			const Vec2 textPos(70, 40);
			const ColorF settingTextColor = Palette::Red;
			const ColorF shadowColor(0, 0, 0, 0.5);
			const Vec2 shadowOffset(2, 5);
			const int settingSize = 40;
			font(U"SETTING").draw(settingSize, textPos + shadowOffset, shadowColor);
			font(U"SETTING").draw(settingSize, textPos, settingTextColor);

			// ボタン描画
			for (auto& b : buttons)
			{
				b.draw(font);
			}
		}
	};
}
