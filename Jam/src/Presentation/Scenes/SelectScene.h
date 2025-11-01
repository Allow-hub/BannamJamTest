#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14
#include "TitleScene.h"
#include "../../Foundation/CoreManager.h"
#include "TransitionManager.h"
#include "../AudioService.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;
	class SelectScene : public App::Scene
	{
	private:
		enum class State
		{
			WorldSelect,
			StageSelect,
		};

		State m_state = State::WorldSelect;
		int32 m_selectedWorld = 0;

		s3d::Texture m_worldBackgroundTexture;
		s3d::Texture m_stageBackgroundTexture;
		s3d::Texture m_worldButtonTexture_01;
		s3d::Texture m_worldButtonTexture_02;
		s3d::Texture m_worldButtonTexture_03;

		s3d::Texture m_stageButtonTexture_01;
		s3d::Texture m_stageButtonTexture_02;
		s3d::Texture m_stageButtonTexture_03;

		s3d::Texture m_backButtonTexture;

	public:
		// コンストラクタ
		SelectScene(const InitData& init)
			: IScene{ init }
		{
			FontAsset::Register(U"SelectTitleFont", 60, Typeface::Bold);
			FontAsset::Register(U"SelectMenuFont", 40, Typeface::Bold);
			FontAsset::Register(U"SelectStageFont", 30, Typeface::Regular);

			// 画像の読み込み
			m_worldBackgroundTexture = s3d::Texture{ U"Assets/Select/serect-AREA_back-screen.png" };
			m_stageBackgroundTexture = s3d::Texture{ U"Assets/Select/serect-stage_back-screen.png" };
			m_worldButtonTexture_01 = s3d::Texture{ U"Assets/Select/area_01.png" };
			m_worldButtonTexture_02 = s3d::Texture{ U"Assets/Select/area_02.png" };
			m_worldButtonTexture_03 = s3d::Texture{ U"Assets/Select/area_03.png" };
			m_stageButtonTexture_01 = s3d::Texture{ U"Assets/Select/stage1_1.png" };
			m_stageButtonTexture_02 = s3d::Texture{ U"Assets/Select/stage1_2.png" };
			m_stageButtonTexture_03 = s3d::Texture{ U"Assets/Select/stage1_3.png" };

			m_backButtonTexture = s3d::Texture{ U"Assets/Select/stage-serect_BACK.png" };
			Jam::Foundation::CoreManager::Instance().setClear(false);
		}

		void update() override
		{
			switch (m_state)
			{
			case State::WorldSelect:
				updateWorldSelect();
				break;
			case State::StageSelect:
				updateStageSelect();
				break;
			}

			// ポーズ処理
			if (KeyEscape.down())
			{
				Jam::Foundation::CoreManager::Instance().setPause(!Jam::Foundation::CoreManager::Instance().getPause());
			}
			if (Jam::Foundation::CoreManager::Instance().getPause())
			{
				Jam::Presentation::SettingManager::Instance().update();
				return;
			}
		}

		void draw() const override
		{
			switch (m_state)
			{
			case State::WorldSelect:
				m_worldBackgroundTexture.resized(Scene::Size()).draw(0, 0);
				drawWorldSelect();
				break;
			case State::StageSelect:
				m_stageBackgroundTexture.resized(Scene::Size()).draw(0, 0);
				drawStageSelect();
				break;
			}
			if (Jam::Foundation::CoreManager::Instance().getPause())
			{
				Jam::Presentation::SettingManager::Instance().draw();
				return;
			}
		}

		// シーンがフェードインする（現れる）ときの描画
		void drawFadeIn(double t) const override
		{
			draw();
			Jam::Presentation::Scenes::TransitionManager::Instance().rec.drawFadeIn(t);
		}

		// シーンがフェードアウトする（消える）ときの描画
		void drawFadeOut(double t) const override
		{
			draw();
			Jam::Presentation::Scenes::TransitionManager::Instance().rec.drawFadeOut(t);
		}

	private:
		// --- ワールド選択のロジック ---
		void updateWorldSelect()
		{
			// --- 画面サイズに基づいた相対的な値 ---
			const double baseWidth = 1920.0;
			const double baseHeight = 1080.0;

			const double buttonWidth = Scene::Width() * (400.0 / baseWidth);
			const double buttonHeight = Scene::Height() * (150.0 / baseHeight);
			const double buttonSpacing = Scene::Height() * (200.0 / baseHeight);
			const double startX = Scene::Width() - (Scene::Width() * (490.0 / baseWidth));
			const double startY = Scene::Height() * (200.0 / baseHeight);

			// --- Area01（上） ---
			const RectF area01{ startX, startY + (buttonHeight + buttonSpacing) * 0, buttonWidth, buttonHeight };
			if (area01.leftClicked())
			{
				AudioService::get().playOneShot(AudioService::Sound::SE_Button, AudioService::VOLUME_BUTTON);
				m_selectedWorld = 1;
				m_state = State::StageSelect;
			}

			// --- Area02（中） ---
			const RectF area02{ startX, startY + (buttonHeight + buttonSpacing) * 1, buttonWidth, buttonHeight };
			if (area02.leftClicked())
			{
				AudioService::get().playOneShot(AudioService::Sound::SE_Button, AudioService::VOLUME_BUTTON);
				//m_selectedWorld = 2;
				//m_state = State::StageSelect;
			}

			// --- Area03（下） ---
			const RectF area03{ startX, startY + (buttonHeight + buttonSpacing) * 2, buttonWidth, buttonHeight };
			if (area03.leftClicked())
			{
				AudioService::get().playOneShot(AudioService::Sound::SE_Button, AudioService::VOLUME_BUTTON);
				//m_selectedWorld = 3;
				//m_state = State::StageSelect;
			}
		}


		// --- ワールド選択の描画 ---
		void drawWorldSelect() const
		{
			const double baseWidth = 1920.0;
			const double baseHeight = 1080.0;

			const double buttonWidth = Scene::Width() * (400.0 / baseWidth);
			const double buttonHeight = Scene::Height() * (150.0 / baseHeight);
			const double buttonSpacing = Scene::Height() * (200.0 / baseHeight);
			const double startX = Scene::Width() - (Scene::Width() * (490.0 / baseWidth));
			const double startY = Scene::Height() * (200.0 / baseHeight);

			// --- 上から順に 01 → 02 → 03 ---
			const RectF area01{ startX, startY + (buttonHeight + buttonSpacing) * 0, buttonWidth, buttonHeight };
			const RectF area02{ startX, startY + (buttonHeight + buttonSpacing) * 1, buttonWidth, buttonHeight };
			const RectF area03{ startX, startY + (buttonHeight + buttonSpacing) * 2, buttonWidth, buttonHeight };

			drawButton(area01, U"", area01.mouseOver(), m_worldButtonTexture_01);
			drawButton(area02, U"", area02.mouseOver(), m_worldButtonTexture_02);
			drawButton(area03, U"", area03.mouseOver(), m_worldButtonTexture_03);
		}


		// --- ステージ選択のロジック ---
		void updateStageSelect()
		{
			// --- 基準解像度 (1920 x 1080) ---
			const double baseWidth = 1920.0;
			const double baseHeight = 1080.0;

			// 「戻る」ボタンのクリック判定
			const RectF backButton{ 0, 0, Scene::Width() * (250.0 / baseWidth), Scene::Height() * (80.0 / baseHeight) };
			if (backButton.leftClicked())
			{
				AudioService::get().playOneShot(AudioService::Sound::SE_Button, AudioService::VOLUME_BUTTON);
				m_state = State::WorldSelect;
			}

			// 画面右側に縦に並べる
			// --- 画面サイズに基づいた相対的な値 ---
			const double buttonWidth = Scene::Width() * (400.0 / baseWidth);
			const double buttonHeight = Scene::Height() * (150.0 / baseHeight);
			const double buttonSpacing = Scene::Height() * (200.0 / baseHeight);
			const double startX = Scene::Width() - (Scene::Width() * (490.0 / baseWidth));
			const double startY = Scene::Height() * (200.0 / baseHeight);
			// --- ここまで ---

			// 選択中のワールドに応じて処理を分岐
			if (m_selectedWorld == 1)
			{
				const RectF stage1_1_Button{ startX, startY + (buttonHeight + buttonSpacing) * 0, buttonWidth, buttonHeight };
				if (stage1_1_Button.leftClicked())
				{
					AudioService::get().playOneShot(AudioService::Sound::SE_Button, AudioService::VOLUME_BUTTON);
					Jam::Foundation::CoreManager::Instance().stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;
					changeScene(ToSceneString(SceneName::Story), 1.0s);
				}

				const RectF stage1_2_Button{ startX, startY + (buttonHeight + buttonSpacing) * 1, buttonWidth, buttonHeight };
				if (stage1_2_Button.leftClicked())
				{
					AudioService::get().playOneShot(AudioService::Sound::SE_Button, AudioService::VOLUME_BUTTON);
					Jam::Foundation::CoreManager::Instance().stageInfo.stageName = Jam::Foundation::StageName::Stage1_2;
					changeScene(ToSceneString(SceneName::Story), 1.0s);
				}

				const RectF stage1_3_Button{ startX, startY + (buttonHeight + buttonSpacing) * 2, buttonWidth, buttonHeight };
				if (stage1_3_Button.leftClicked())
				{
					AudioService::get().playOneShot(AudioService::Sound::SE_Button, AudioService::VOLUME_BUTTON);
					Jam::Foundation::CoreManager::Instance().stageInfo.stageName = Jam::Foundation::StageName::Stage1_3;
					changeScene(ToSceneString(SceneName::Story), 1.0s);
				}
			}
			else if (m_selectedWorld == 2)
			{
				// (ここにWorld 2のステージボタンのロジックを同様に記述)
			}
		}

		// --- ステージ選択の描画 ---
		void drawStageSelect() const
		{
			// --- 基準解像度 (1920 x 1080) ---
			const double baseWidth = 1920.0;
			const double baseHeight = 1080.0;

			// 「戻る」ボタンの描画
			// 元のピクセル値: const RectF backButton{ 0, 0, 250, 80 };
			const RectF backButton{ 0, 0, Scene::Width() * (250.0 / baseWidth), Scene::Height() * (80.0 / baseHeight) };
			drawButton(backButton, U"", backButton.mouseOver(), m_backButtonTexture);

			// --- 画面サイズに基づいた相対的な値 ---
			const double buttonWidth = Scene::Width() * (400.0 / baseWidth);
			const double buttonHeight = Scene::Height() * (150.0 / baseHeight);
			const double buttonSpacing = Scene::Height() * (200.0 / baseHeight);
			const double startX = Scene::Width() - (Scene::Width() * (490.0 / baseWidth));
			const double startY = Scene::Height() * (200.0 / baseHeight);
			

			// 選択中のワールドに応じてボタンを描画
			if (m_selectedWorld == 1)
			{
				const RectF stage1_1_Button{ startX, startY + (buttonHeight + buttonSpacing) * 0, buttonWidth, buttonHeight };
				drawButton(stage1_1_Button, U"", stage1_1_Button.mouseOver(), m_stageButtonTexture_01, U"SelectStageFont");

				const RectF stage1_2_Button{ startX, startY + (buttonHeight + buttonSpacing) * 1, buttonWidth, buttonHeight };
				drawButton(stage1_2_Button, U"", stage1_2_Button.mouseOver(), m_stageButtonTexture_02, U"SelectStageFont");

				const RectF stage1_3_Button{ startX, startY + (buttonHeight + buttonSpacing) * 2, buttonWidth, buttonHeight };
				drawButton(stage1_3_Button, U"", stage1_3_Button.mouseOver(), m_stageButtonTexture_03, U"SelectStageFont");
			}
			else if (m_selectedWorld == 2)
			{
				// (ここにWorld 2のステージボタンの描画を同様に記述)
			}
		}

		// --- (共通) ボタン描画ヘルパー関数 ---
		void drawButton(const RectF& rect, const String& text, bool mouseOver, const s3d::Texture& buttonTexture, const String& fontName = U"SelectMenuFont") const
		{
			// 背景画像を描画
			buttonTexture.resized(rect.size).draw(rect.pos);

			if (mouseOver)
			{
				// マウスオーバー時のエフェクト (半透明の白を重ねる)
				rect.draw(ColorF{ 1.0, 0.4 });
				Cursor::RequestStyle(CursorStyle::Hand);
			}

			// テキストを描画
			FontAsset(fontName)(text).drawAt(rect.center(), ColorF{ 0.2 });
		}
	};
}
