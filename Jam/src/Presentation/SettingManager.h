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

		// SceneManagerへのポインタ
		SceneManager<String>* m_sceneManager = nullptr;


		struct Button
		{
			String label;
			Vec2 pos;
			int size;
			RectF rect;
			double underlineWidth = 0.0;
			double underlineTarget = 0.0;

			Button(const String& l, const Vec2& p, int s, double maxWidth)
				: label(l), pos(p), size(s), underlineTarget(maxWidth)
			{
				rect = RectF(pos, Vec2(200, s));
			}

			bool isHovered() const { return rect.mouseOver(); }

			void update()
			{
				const double speed = 100.0;
				if (isHovered())
				{
					underlineWidth = std::min(underlineWidth + speed, underlineTarget);
				}
				else
				{
					underlineWidth = std::max(underlineWidth - speed, 0.0);
				}
			}

			void draw(const Font& font)
			{
				const double underlineHeight = 35.0;
				const Vec2 underlinePos = pos + Vec2(-10, size - 28);
				RectF(underlinePos, Vec2(underlineWidth, underlineHeight)).draw({ 0.957, 0.192, 0.349 });

				const double w = font(label).region().w * (size / 40.0);
				rect = RectF(pos, Vec2(w, size));
				font(label).draw(size, pos, Palette::White);
			}
		};

		Array<Button> buttons;

	public:
		static SettingManager& Instance()
		{
			static SettingManager instance;
			return instance;
		}

		// SceneManagerを設定
		void setSceneManager(SceneManager<String>* manager)
		{
			m_sceneManager = manager;
		}

		// シーン遷移（SceneName enum を使用）
		void changeScene(Scenes::SceneName scene)
		{
			if (m_sceneManager)
			{
				m_sceneManager->changeScene(Scenes::ToSceneString(scene));
			}
		}

		// シーン遷移（String を直接使用）
		void changeScene(const String& sceneName)
		{
			if (m_sceneManager)
			{
				m_sceneManager->changeScene(sceneName);
			}
		}

		void init()
		{
			m_backgroundTexture = Texture{ U"../Assets/Stage/BG.png" };
			const int32 mainFontSize = static_cast<int32>(Scene::Height() * 0.07);
			font = Font(mainFontSize, U"../Assets/Font/PixelMplus12-Bold.ttf", FontStyle::Bold);
			const double maxUnderline = font(U"BACK THE SELECT").region().w - 100.0;

			buttons.clear();
			buttons.emplace_back(U"AUDIO", Vec2(70, 120), 60, maxUnderline);
			buttons.emplace_back(U"RETRY", Vec2(70, 220), 60, maxUnderline);
			buttons.emplace_back(U"EXIT", Vec2(70, 320), 60, maxUnderline);
			buttons.emplace_back(U"BACK THE SELECT", Vec2(70, 420), 60, maxUnderline);
		}

		void update()
		{
			for (auto& b : buttons)
			{
				b.update();

				if (b.isHovered() && MouseL.down())
				{
					if (b.label == U"AUDIO")
					{
						Print << U"Audio button clicked";
					}
					else if (b.label == U"RETRY")
					{
						changeScene(Scenes::SceneName::InGame);
						Jam::Foundation::CoreManager::Instance().setPause(false);
					}
					else if (b.label == U"EXIT")
					{
						Jam::Foundation::CoreManager::Instance().setPause(false);
					}
					else if (b.label == U"BACK THE SELECT")
					{
						changeScene(Scenes::SceneName::Select);
					}
				}
			}
		}

		void draw()
		{
			m_backgroundTexture.resized(Scene::Size()).draw(0, 0);

			const Vec2 textPos(70, 40);
			const ColorF settingTextColor = Palette::Red;
			const ColorF shadowColor(0, 0, 0, 0.5);
			const Vec2 shadowOffset(2, 5);
			const int settingSize = 40;

			font(U"SETTING").draw(settingSize, textPos + shadowOffset, shadowColor);
			font(U"SETTING").draw(settingSize, textPos, settingTextColor);

			for (auto& b : buttons)
			{
				b.draw(font);
			}
		}
	};
}
