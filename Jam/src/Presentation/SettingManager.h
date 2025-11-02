#pragma once
#include <Siv3D.hpp>
#include "../Infrastructure/IndependentObjectFactory.h"

namespace Jam::Presentation
{
	class SettingManager
	{
	private:
		SettingManager() = default;
		Font font;
		Texture m_backgroundTexture;
		Texture m_backgroundMaskTexture;
		Texture m_creditTexture;

		SceneManager<String>* m_sceneManager = nullptr;

		enum class Mode { Menu, Audio, Credit };
		Mode m_mode = Mode::Menu;

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
					underlineWidth = std::min(underlineWidth + speed, underlineTarget);
				else
					underlineWidth = std::max(underlineWidth - speed, 0.0);
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

		void setSceneManager(SceneManager<String>* manager)
		{
			m_sceneManager = manager;
		}

		void changeScene(const String& sceneName)
		{
			Jam::Infrastructure::IndependentObjectFactory::instance().clearAllObjects();
			if (m_sceneManager)
				m_sceneManager->changeScene(sceneName);
		}

		void init()
		{
			m_backgroundTexture = Texture{ Resource(U"Assets/setting_backMain.png") };
			m_backgroundMaskTexture = Texture{ Resource(U"Assets/setting_back.png") };
			m_creditTexture = Texture{ Resource(U"Assets/Credit.png") };
			const int32 mainFontSize = static_cast<int32>(Scene::Height() * 0.07);
			font = Font(mainFontSize, U"Assets/Font/PixelMplus12-Bold.ttf", FontStyle::Bold);
			const double maxUnderline = font(U"BACK THE SELECT").region().w - 100.0;

			buttons.clear();
			buttons.emplace_back(U"AUDIO", Vec2(70, 120), 60, maxUnderline);
			buttons.emplace_back(U"RETRY", Vec2(70, 220), 60, maxUnderline);
			buttons.emplace_back(U"EXIT", Vec2(70, 320), 60, maxUnderline);
			buttons.emplace_back(U"BACK THE SELECT", Vec2(70, 420), 60, maxUnderline);
			buttons.emplace_back(U"CREDIT", Vec2(70, 520), 60, maxUnderline);
			buttons.emplace_back(U"END", Vec2(70, 620), 60, maxUnderline);
		}

		void update()
		{
			if (m_mode == Mode::Menu)
			{
				for (auto& b : buttons)
				{
					b.update();

					if (b.isHovered() && MouseL.down())
					{
						if (b.label == U"AUDIO")
						{
							m_mode = Mode::Audio;
						}
						else if (b.label == U"CREDIT")
						{
							m_mode = Mode::Credit;
						}
						else if (b.label == U"RETRY")
						{
							changeScene(U"InGame");
							Jam::Foundation::CoreManager::Instance().setPause(false);
						}
						else if (b.label == U"EXIT")
						{
							Jam::Foundation::CoreManager::Instance().setPause(false);
						}
						else if (b.label == U"BACK THE SELECT")
						{
							changeScene(U"Select");
							Jam::Foundation::CoreManager::Instance().setPause(false);
						}
						else if (b.label == U"END")
						{
							System::Exit(); // Siv3Dのアプリを終了
						}
					}
				}
			}
			else if (m_mode == Mode::Audio)
			{
				auto& core = Jam::Foundation::CoreManager::Instance();
				auto& audioService = Jam::Presentation::AudioService::get();

				// スライダーの値を保存（変更検知用）
				static double prevMaster = core.audioSetting.masterVolume;
				static double prevBGM = core.audioSetting.bgmVolume;
				static double prevSE = core.audioSetting.seVolume;

				// 値が変更されたらAudioServiceに反映
				if (prevMaster != core.audioSetting.masterVolume)
				{
					audioService.setMasterVolume(core.audioSetting.masterVolume);
					prevMaster = core.audioSetting.masterVolume;
				}

				if (prevBGM != core.audioSetting.bgmVolume)
				{
					audioService.setBGMVolume(core.audioSetting.bgmVolume);
					prevBGM = core.audioSetting.bgmVolume;
				}

				if (prevSE != core.audioSetting.seVolume)
				{
					audioService.setSEVolume(core.audioSetting.seVolume);
					prevSE = core.audioSetting.seVolume;
				}

				if (SimpleGUI::Button(U"BACK", Vec2(70, 420), 120))
				{
					m_mode = Mode::Menu;
				}
			}
		}

		void draw()
		{
			m_backgroundTexture.resized(Scene::Size()).draw(0, 0);
			m_backgroundMaskTexture.resized(Scene::Size()).draw(0, 0);

			if (m_mode == Mode::Menu)
			{
				const Vec2 textPos(70, 40);
				font(U"SETTING").draw(40, textPos + Vec2(2, 5), ColorF(0, 0, 0, 0.5));
				font(U"SETTING").draw(40, textPos, Palette::Red);

				for (auto& b : buttons)
				{
					b.draw(font);
				}
			}
			else if (m_mode == Mode::Audio)
			{
				const Vec2 textPos(70, 40);
				auto& core = Jam::Foundation::CoreManager::Instance();
				font(U"AUDIO SETTING").draw(40, textPos + Vec2(2, 5), ColorF(0, 0, 0, 0.5));
				font(U"AUDIO SETTING").draw(40, textPos, Palette::Red);

				SimpleGUI::Slider(U"Master", core.audioSetting.masterVolume, Vec2(70, 180), 200, 200, true);
				SimpleGUI::Slider(U"BGM", core.audioSetting.bgmVolume, Vec2(70, 260), 200, 200, true);
				SimpleGUI::Slider(U"SE", core.audioSetting.seVolume, Vec2(70, 340), 200, 200, true);

				SimpleGUI::Button(U"BACK", Vec2(70, 420), 120, true);
			}
			else if (m_mode == Mode::Credit)
			{
				// クレジット画像を表示
				m_creditTexture.resized(Scene::Size()).draw(0, 0);

				// BACKボタン
				if (SimpleGUI::Button(U"BACK", Vec2(70, 620), 120))
				{
					m_mode = Mode::Menu;
				}
			}
		}
	};
}
