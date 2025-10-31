#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14
#include "TitleScene.h"
#include "../../Foundation/CoreManager.h"
#include "TransitionManager.h"

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
		s3d::Texture m_worldButtonTexture;
		s3d::Texture m_stageButtonTexture;
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
			m_worldBackgroundTexture = s3d::Texture{ U"../Assets/Select/serect-AREA_back-screen.png" };
			m_stageBackgroundTexture = s3d::Texture{ U"../Assets/Select/serect-stage_back-screen.png" };
			m_worldButtonTexture = s3d::Texture{ U"../Assets/Select/serect-area_stage01.png" };
			m_stageButtonTexture = s3d::Texture{ U"../Assets/Select/stage-serect_stage1-1.png" };
			m_backButtonTexture = s3d::Texture{ U"../Assets/Select/stage-serect_BACK.png" };
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
			// 基準解像度 (1920 x 1080)
			const double baseWidth = 1920.0;
			const double baseHeight = 1080.0;

			const double buttonWidth = Scene::Width() * (400.0 / baseWidth);
			const double buttonHeight = Scene::Height() * (150.0 / baseHeight);
			const double buttonSpacing = Scene::Height() * (200.0 / baseHeight); // 縦のスペーシング
			const double startX = Scene::Width() - (Scene::Width() * (490.0 / baseWidth)); // 右端からのオフセット
			const double startY = Scene::Height() * (200.0 / baseHeight); // 上端からのオフセット

			// World 1 ボタン
			const RectF world1Button{ startX, startY + (buttonHeight + buttonSpacing) * 0, buttonWidth, buttonHeight };
			if (world1Button.leftClicked())
			{
				Print << U"World 1 Selected";
				m_selectedWorld = 1;
				m_state = State::StageSelect;
			}

			// World 2 ボタン
			const RectF world2Button{ startX, startY + (buttonHeight + buttonSpacing) * 1, buttonWidth, buttonHeight };
			if (world2Button.leftClicked())
			{
				Print << U"World 2 Selected";
				m_selectedWorld = 2;
				m_state = State::StageSelect;
			}

			// World 3 ボタン
			const RectF world3Button{ startX, startY + (buttonHeight + buttonSpacing) * 2, buttonWidth, buttonHeight };
			if (world3Button.leftClicked())
			{
				Print << U"World 3 Selected";
				m_selectedWorld = 3;
				m_state = State::StageSelect;
			}
		}

		// --- ワールド選択の描画 ---
		void drawWorldSelect() const
		{
			// --- 画面サイズに基づいた相対的な値 ---
			// 基準解像度 (1920 x 1080)
			const double baseWidth = 1920.0;
			const double baseHeight = 1080.0;

			const double buttonWidth = Scene::Width() * (400.0 / baseWidth);
			const double buttonHeight = Scene::Height() * (150.0 / baseHeight);
			const double buttonSpacing = Scene::Height() * (200.0 / baseHeight); // 縦のスペーシング
			const double startX = Scene::Width() - (Scene::Width() * (490.0 / baseWidth)); // 右端からのオフセット
			const double startY = Scene::Height() * (200.0 / baseHeight); // 上端からのオフセット
			// --- ここまで ---

			// World 1 ボタン
			const RectF world1Button{ startX, startY + (buttonHeight + buttonSpacing) * 0, buttonWidth, buttonHeight };
			drawButton(world1Button, U"", world1Button.mouseOver(), m_worldButtonTexture);

			// World 2 ボタン
			const RectF world2Button{ startX, startY + (buttonHeight + buttonSpacing) * 1, buttonWidth, buttonHeight };
			drawButton(world2Button, U"", world2Button.mouseOver(), m_worldButtonTexture);

			// World 3 ボタン
			const RectF world3Button{ startX, startY + (buttonHeight + buttonSpacing) * 2, buttonWidth, buttonHeight };
			drawButton(world3Button, U"", world3Button.mouseOver(), m_worldButtonTexture);
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
				Print << U"Back to World Select";
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
					Jam::Foundation::CoreManager::Instance().stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;
					Print << U"Stage 1-1 Selected. CoreManager set.";
					changeScene(ToSceneString(SceneName::Story), 1.0s);
				}

				const RectF stage1_2_Button{ startX, startY + (buttonHeight + buttonSpacing) * 1, buttonWidth, buttonHeight };
				if (stage1_2_Button.leftClicked())
				{
					Jam::Foundation::CoreManager::Instance().stageInfo.stageName = Jam::Foundation::StageName::Stage1_2;
					Print << U"Stage 1-2 Selected. CoreManager set.";
					changeScene(ToSceneString(SceneName::Story), 1.0s);
				}

				const RectF stage1_3_Button{ startX, startY + (buttonHeight + buttonSpacing) * 2, buttonWidth, buttonHeight };
				if (stage1_3_Button.leftClicked())
				{
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
				drawButton(stage1_1_Button, U"", stage1_1_Button.mouseOver(), m_stageButtonTexture, U"SelectStageFont");

				const RectF stage1_2_Button{ startX, startY + (buttonHeight + buttonSpacing) * 1, buttonWidth, buttonHeight };
				drawButton(stage1_2_Button, U"", stage1_2_Button.mouseOver(), m_stageButtonTexture, U"SelectStageFont");

				const RectF stage1_3_Button{ startX, startY + (buttonHeight + buttonSpacing) * 2, buttonWidth, buttonHeight };
				drawButton(stage1_3_Button, U"", stage1_3_Button.mouseOver(), m_stageButtonTexture, U"SelectStageFont");
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
