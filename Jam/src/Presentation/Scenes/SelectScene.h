#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14
#include "TitleScene.h"
#include "../../Foundation/CoreManager.h"

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

		Jam::Foundation::CoreManager* m_coreManager;

	public:
		// コンストラクタ
		SelectScene(const InitData& init)
			: IScene{ init }
		{
			FontAsset::Register(U"SelectTitleFont", 60, Typeface::Bold);

			FontAsset::Register(U"SelectMenuFont", 40, Typeface::Bold);
			FontAsset::Register(U"SelectStageFont", 30, Typeface::Regular);

			m_coreManager = &Jam::Foundation::CoreManager::Instance();
		}

		// 更新処理
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

		// 描画処理
		void draw() const override
		{
			//Scene::SetBackground(ColorF{ 0.9, 0.9, 1.0 });
			Scene::SetBackground(ColorF{ 0.3, 0.6, 0.8 });

			switch (m_state)
			{
			case State::WorldSelect:
				drawWorldSelect();
				break;
			case State::StageSelect:
				drawStageSelect();
				break;
			}
		}
	private:
		// ===================================
		// ▼▼▼ 以下、すべて追記 ▼▼▼
		// ===================================

		// --- ワールド選択のロジック ---
		void updateWorldSelect()
		{
			// 「ワールド1」ボタン
			const RectF world1Button{ Arg::center = Scene::Center().movedBy(0, 0), 300, 80 };
			if (world1Button.leftClicked())
			{
				Print << U"World 1 Selected"; // デバッグログ
				m_selectedWorld = 1;
				m_state = State::StageSelect; // ステージ選択へ遷移
			}

			// 「ワールド2」ボタン（拡張性のデモ）
			const RectF world2Button{ Arg::center = Scene::Center().movedBy(0, 100), 300, 80 };
			if (world2Button.leftClicked())
			{
				Print << U"World 2 Selected"; // デバッグログ
				m_selectedWorld = 2;
				m_state = State::StageSelect; // ステージ選択へ遷移
			}
		}

		// --- ワールド選択の描画 ---
		void drawWorldSelect() const
		{
			FontAsset(U"SelectTitleFont")(U"ワールド選択").drawAt(Scene::Center().x, 100);

			// 「ワールド1」ボタンの描画
			const RectF world1Button{ Arg::center = Scene::Center().movedBy(0, 0), 300, 80 };
			drawButton(world1Button, U"World 1", world1Button.mouseOver());

			// 「ワールド2」ボタンの描画
			const RectF world2Button{ Arg::center = Scene::Center().movedBy(0, 100), 300, 80 };
			drawButton(world2Button, U"World 2", world2Button.mouseOver());
		}

		// --- ステージ選択のロジック ---
		void updateStageSelect()
		{
			// 「戻る」ボタン
			const RectF backButton{ Arg::center = Scene::Center().movedBy(0, 300), 150, 50 };
			if (backButton.leftClicked())
			{
				Print << U"Back to World Select";
				m_state = State::WorldSelect; // ワールド選択へ戻る
			}

			// 選択中のワールドに応じて処理を分岐
			if (m_selectedWorld == 1)
			{
				// 「1-1」ボタン
				const RectF stage1_1_Button{ Arg::center = Scene::Center().movedBy(-80, 0), 120, 60 };
				if (stage1_1_Button.leftClicked())
				{
					// 要件：CoreManagerのStageInfoを設定
					m_coreManager->stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;
					// 要件：デバッグログ
					Print << U"Stage 1-1 Selected. CoreManager set.";
					// 要件：InGameシーンへ移動
					changeScene(ToSceneString(SceneName::InGame));
				}

				// 「1-2」ボタン
				const RectF stage1_2_Button{ Arg::center = Scene::Center().movedBy(80, 0), 120, 60 };
				if (stage1_2_Button.leftClicked())
				{
					m_coreManager->stageInfo.stageName = Jam::Foundation::StageName::Stage1_2;
					Print << U"Stage 1-2 Selected. CoreManager set.";
					changeScene(ToSceneString(SceneName::InGame));
				}
			}
			else if (m_selectedWorld == 2)
			{
				// 「2-1」ボタン
				const RectF stage2_1_Button{ Arg::center = Scene::Center().movedBy(-80, 0), 120, 60 };
				if (stage2_1_Button.leftClicked())
				{
					// m_coreManager->stageInfo.stageName = Jam::Foundation::StageName::Stage2_1; // CoreManagerのenumに追加が必要
					Print << U"Stage 2-1 Selected. (Not Implemented)";
					// changeScene(ToSceneString(SceneName::InGame));
				}
			}
		}

		// --- ステージ選択の描画 ---
		void drawStageSelect() const
		{
			// U"ステージ選択 (World 1)" のように描画
			FontAsset(U"SelectTitleFont")(U"ステージ選択 (World {})"_fmt(m_selectedWorld)).drawAt(Scene::Center().x, 100);

			// 「戻る」ボタンの描画
			const RectF backButton{ Arg::center = Scene::Center().movedBy(0, 300), 150, 50 };
			drawButton(backButton, U"戻る", backButton.mouseOver());

			// 選択中のワールドに応じてボタンを描画
			if (m_selectedWorld == 1)
			{
				const RectF stage1_1_Button{ Arg::center = Scene::Center().movedBy(-80, 0), 120, 60 };
				drawButton(stage1_1_Button, U"1-1", stage1_1_Button.mouseOver(), U"SelectStageFont");

				const RectF stage1_2_Button{ Arg::center = Scene::Center().movedBy(80, 0), 120, 60 };
				drawButton(stage1_2_Button, U"1-2", stage1_2_Button.mouseOver(), U"SelectStageFont");
			}
			else if (m_selectedWorld == 2)
			{
				const RectF stage2_1_Button{ Arg::center = Scene::Center().movedBy(-80, 0), 120, 60 };
				drawButton(stage2_1_Button, U"2-1", stage2_1_Button.mouseOver(), U"SelectStageFont");
			}
		}

		// --- (共通) ボタン描画ヘルパー関数 ---
		void drawButton(const RectF& rect, const String& text, bool mouseOver, const String& fontName = U"SelectMenuFont") const
		{
			if (mouseOver)
			{
				rect.draw(ColorF{ 0.9 });
				rect.drawFrame(2, 0, ColorF{ 0.6 });
				Cursor::RequestStyle(CursorStyle::Hand);
			}
			else
			{
				rect.draw();
				rect.drawFrame(2, 0, ColorF{ 0.7 });
			}
			FontAsset(fontName)(text).drawAt(rect.center(), ColorF{ 0.2 });
		}
	};
}
