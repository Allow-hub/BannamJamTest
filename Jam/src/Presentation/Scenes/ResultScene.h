#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14
#include "TitleScene.h"
#include "SelectScene.h"
#include "../../Foundation/CoreManager.h"
#include "../../Presentation/SettingManager.h"
#include "TransitionManager.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;
	class ResultScene : public App::Scene
	{
	private:
		// --- メンバ変数の宣言 ---
		bool m_isClear;
		int m_flagmentAmount;
		int m_maxFlagment;
		int m_defeatedCount;
		unsigned int m_time;

		// --- フォント ---
		Font m_titleFont;
		Font m_statLabelFont;
		Font m_statValueFont;
		Font m_buttonFont;

		// --- テクスチャ ---
		Texture m_background;
		// Texture m_chatBg; // (削除)
		// Texture m_iconRisuka; // (削除)
		// Texture m_iconYumemi; // (削除)
		Texture m_titleMemory;
		Texture m_titleTime;
		Texture m_titleKill;
		Texture m_titleGameOver;
		Texture m_titleClear;

		// (追加) ステージ結果表示用のテクスチャ
		Texture m_stageResultTexture;

		// --- カスタムボタン用の Rect ---
		RectF m_retryButton;
		RectF m_mapButton;
		RectF m_nextStageButton;

		double m_retryBarWidth = 0.0;
		double m_mapBarWidth = 0.0;
		double m_nextStageBarWidth = 0.0;

		const Color m_statColor = Color(164, 179, 255);

	public:
		// コンストラクタ
		ResultScene(const InitData& init)
			: IScene{ init }
		{
			// --- 基準解像度 (1920 x 1080) ---
			const double baseWidth = 1920.0;
			const double baseHeight = 1080.0;

			auto& core = Jam::Foundation::CoreManager::Instance();
			m_isClear = core.getClear();
			m_flagmentAmount = core.getFlagment();
			m_maxFlagment = core.getMaxFlagment();
			m_defeatedCount = core.getDefeatedEnemyCount();
			m_time = static_cast<unsigned int>(core.getTimer());

			// --- フォントの読み込み (すべて相対化) ---
			m_titleFont = Font(static_cast<int32>(Scene::Height() * (100.0 / baseHeight)), Typeface::Heavy);
			m_statLabelFont = Font(static_cast<int32>(Scene::Height() * (30.0 / baseHeight)), Typeface::Bold);

			m_statValueFont = Font(static_cast<int32>(Scene::Height() * (130.0 / baseHeight)), U"Assets/Font/PixelMplus12-Bold.ttf");
			if (not m_statValueFont)
			{
				Print(U"Error: StatValueFont 'PixelMplus12-Bold.ttf' not found. Using default.");
				m_statValueFont = Font(static_cast<int32>(Scene::Height() * (130.0 / baseHeight)), Typeface::Bold);
			}

			const int32 mainFontSize = static_cast<int32>(Scene::Height() * 0.07);
			m_buttonFont = Font(mainFontSize, U"Assets/Font/PixelMplus12-Bold.ttf");
			if (not m_buttonFont)
			{
				Print(U"Error: ButtonFont 'PixelMplus12-Bold.ttf' not found. Using default.");
				m_buttonFont = Font(mainFontSize, Typeface::Bold);
			}

			// カスタムボタンの座標を初期化 (すべて相対化)
			const double buttonWidth = Scene::Width() * (400.0 / baseWidth);
			const double buttonX = Scene::Center().x - (buttonWidth / 2.0);
			const double buttonHeight = Scene::Height() * (80.0 / baseHeight);
			const double buttonYBase = Scene::Height() * (650.0 / baseHeight);
			const double buttonMargin = Scene::Height() * (90.0 / baseHeight);

			m_retryButton = RectF(buttonX, buttonYBase, buttonWidth, buttonHeight);
			m_mapButton = RectF(buttonX, buttonYBase + buttonMargin, buttonWidth, buttonHeight);
			m_nextStageButton = RectF(buttonX, buttonYBase + buttonMargin * 2, buttonWidth, buttonHeight);


			// === テクスチャ読み込み ===
			const FilePath assetRoot = FileSystem::InitialDirectory() + U"Assets/Result/";

			m_background = Texture(assetRoot + U"GAME_OVER_screen.png");
			// m_chatBg = Texture(assetRoot + U"CLEAR_backscreen.png"); // (削除)
			// m_iconRisuka = Texture(assetRoot + U"ICON-RISUKA.png"); // (削除)
			// m_iconYumemi = Texture(assetRoot + U"ICON-YUMEMI.png"); // (削除)
			m_titleGameOver = Texture(assetRoot + U"Result_Game_Over.png");
			m_titleClear = Texture(assetRoot + U"Result_Crear.png");
			m_titleMemory = Texture(assetRoot + U"result_title-MEMORY.png");
			m_titleTime = Texture(assetRoot + U"result_TIME.png");
			m_titleKill = Texture(assetRoot + U"result_KILL.png");

			// --- (追加) ステージ結果画像の読み込み分岐 ---
			String resultImageName;
			// core.stageInfo.stageName (StageName enum) を使って分岐
			switch (core.stageInfo.stageName)
			{
			case Jam::Foundation::StageName::Stage1_1:
				resultImageName = m_isClear ? U"1_1_clear.png" : U"1_1_miss.png";
				break;
			case Jam::Foundation::StageName::Stage1_2:
				resultImageName = m_isClear ? U"1_2_clear.png" : U"1_2_miss.png";
				break;
			case Jam::Foundation::StageName::Stage1_3:
				resultImageName = m_isClear ? U"1_3_clear.png" : U"1_3_miss.png";
				break;
			default:
				// (念のため) Stage1_1 の画像をフォールバックとして使用
				resultImageName = m_isClear ? U"1_1_clear.png" : U"1_1_miss.png";
				Print << U"Warning: Unknown stageName. Defaulting to Stage 1_1 image.";
				break;
			}

			m_stageResultTexture = Texture(assetRoot + resultImageName);
			// --- ここまで追加 ---


			// === フォールバック ===
			if (not m_background) { m_background = Texture(U"example/city.png"); }
			// if (not m_chatBg) { m_chatBg = Texture(U"example/white_rect.png"); } // (削除)
			// if (not m_iconRisuka) { m_iconRisuka = Texture(U"example/siv3d-kun.png"); } // (削除)
			// if (not m_iconYumemi) { m_iconYumemi = Texture(U"example/siv3d-kun.png"); } // (削除)
			if (not m_titleGameOver) { Print(U"Error: Result_Game_Over.png not found!"); }
			if (not m_titleClear) { Print(U"Error: Result_Crear.png not found!"); }
			if (not m_titleMemory) { Print(U"Error: result_title-MEMORY.png not found!"); }
			if (not m_titleTime) { Print(U"Error: result_TIME.png not found!"); }
			if (not m_titleKill) { Print(U"Error: result_KILL.png not found!"); }

			// (追加) ステージ結果画像のフォールバック
			if (not m_stageResultTexture)
			{
				Print(U"Error: " + resultImageName + U" not found! Using example rect.");
				m_stageResultTexture = Texture(U"example/white_rect.png");
			}
		}


		// 更新処理
		void update() override
		{
			// 1. RETRY ボタン
			if (m_retryButton.leftClicked())
			{
				changeScene(ToSceneString(SceneName::InGame));
			}

			// 2. MAP ボタン
			if (m_mapButton.leftClicked())
			{
				Jam::Foundation::CoreManager::Instance().setNextStagePressed(false);
				if (m_isClear)
					changeScene(ToSceneString(SceneName::Story));
				else
					changeScene(ToSceneString(SceneName::Select));
			}

			// 3. NEXT STAGE ボタン
			if (m_nextStageButton.leftClicked())
			{
				if (m_isClear)
				{
					Jam::Foundation::CoreManager::Instance().setNextStagePressed(true);
					changeScene(ToSceneString(SceneName::Story));
				}
				else
				{
					changeScene(ToSceneString(SceneName::Select));
				}
			}
			// 帯の最大幅（ボタンの横幅と同じ）
			const double targetWidth = m_retryButton.w;
			const double speed = targetWidth * 4.0;

			// --- RETRY ボタン アニメーション ---
			if (m_retryButton.mouseOver())
				m_retryBarWidth = std::min(m_retryBarWidth + Scene::DeltaTime() * speed, targetWidth);
			else
				m_retryBarWidth = std::max(m_retryBarWidth - Scene::DeltaTime() * speed, 0.0);

			// --- MAP ボタン アニメーション ---
			if (m_mapButton.mouseOver())
				m_mapBarWidth = std::min(m_mapBarWidth + Scene::DeltaTime() * speed, targetWidth);
			else
				m_mapBarWidth = std::max(m_mapBarWidth - Scene::DeltaTime() * speed, 0.0);

			// --- NEXT STAGE ボタン アニメーション ---
			if (m_nextStageButton.mouseOver())
				m_nextStageBarWidth = std::min(m_nextStageBarWidth + Scene::DeltaTime() * speed, targetWidth);
			else
				m_nextStageBarWidth = std::max(m_nextStageBarWidth - Scene::DeltaTime() * speed, 0.0);

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

		// 描画処理 
		void draw() const override
		{
			// --- 基準解像度 (1920 x 1080) ---
			const double baseWidth = 1920.0;
			const double baseHeight = 1080.0;

			// (★追加) 統計情報の一番下のY座標を保持する変数
			double statBottomY = 0.0;

			// 1. 背景の描画
			m_background.scaled(Max(Scene::Size().x / (double)m_background.width(), Scene::Size().y / (double)m_background.height()))
				.draw(0, 0, ColorF(0.8));

			// 2. タイトルの描画
			const Vec2 titleCenter = Scene::Center().movedBy(0, Scene::Height() * (-200.0 / baseHeight));

			if (m_isClear)
			{
				if (m_titleClear)
				{
					m_titleClear.drawAt(titleCenter, ColorF(Palette::White));
				}
				else
				{
					m_titleFont(U"CLEAR").drawAt(titleCenter, ColorF(Palette::White));
				}
			}
			else
			{
				if (m_titleGameOver)
				{
					m_titleGameOver.drawAt(titleCenter, ColorF(Palette::White));
				}
				else
				{
					m_titleFont(U"GAME OVER").drawAt(titleCenter, ColorF(Palette::White));
				}
			}

			// 3. 統計情報（左側）
			const double leftStatX = Scene::Width() * (50.0 / baseWidth);
			const double labelYOffset = Scene::Height() * (-20.0 / baseHeight);
			const double statYOffset = Scene::Height() * (200.0 / baseHeight);
			const double blockSpacing = Scene::Height() * (170.0 / baseHeight);
			const double numberVerticalAdjustment = Scene::Height() * (20.0 / baseHeight);
			const double statNumberOffsetX = Scene::Width() * (20.0 / baseWidth);

			const double numberScaleY = 0.7;
			const double numberScaleX = 1.0;
			const Vec2 numberScale = Vec2(numberScaleX, numberScaleY);

			double currentY = statYOffset + Scene::Height() * (250.0 / baseHeight);

			// FRAGMENTS OF MEMORY
			if (m_titleMemory) m_titleMemory.draw(leftStatX, currentY + labelYOffset);
			{
				const Vec2 drawPos = Vec2(leftStatX + statNumberOffsetX, currentY + labelYOffset + numberVerticalAdjustment + m_titleMemory.height());
				const Mat3x2 jam_matrix = Mat3x2::Scale(numberScale) * Mat3x2::Translate(drawPos);
				const Transformer2D jam_scaler(jam_matrix);
				m_statValueFont(U"{}/{}"_fmt(m_flagmentAmount, m_maxFlagment)).draw(Vec2(0, 0), m_statColor);
			}

			// 次のブロックのY座標
			currentY += blockSpacing;

			// TIME
			if (m_titleTime) m_titleTime.draw(leftStatX, currentY + labelYOffset);
			{
				const Vec2 drawPos = Vec2(leftStatX + statNumberOffsetX, currentY + labelYOffset + numberVerticalAdjustment + m_titleTime.height());
				const Mat3x2 jam_matrix = Mat3x2::Scale(numberScale) * Mat3x2::Translate(drawPos);
				const Transformer2D jam_scaler(jam_matrix);
				m_statValueFont(FormatTime(SecondsF(m_time), U"mm:ss")).draw(Vec2(0, 0), m_statColor);
			}

			// 次のブロックのY座標
			currentY += blockSpacing;

			// KILL RACIO
			if (m_titleKill) m_titleKill.draw(leftStatX, currentY + labelYOffset);
			{
				const Vec2 drawPos = Vec2(leftStatX + statNumberOffsetX, currentY + labelYOffset + numberVerticalAdjustment + m_titleKill.height());
				const Mat3x2 jam_matrix = Mat3x2::Scale(numberScale) * Mat3x2::Translate(drawPos);
				const Transformer2D jam_scaler(jam_matrix);

				// 描画するテキストと領域を先に取得 (空だったブロックを修正)
				const auto textToDraw = m_statValueFont(Pad(m_defeatedCount, { 3, U'0' }));
				const auto textRegion = textToDraw.region(Vec2(0, 0)); // スケール前の領域

				textToDraw.draw(Vec2(0, 0), m_statColor); // Transformer2Dが(0,0)をdrawPosに移動させる

				// 下端を計算 (描画位置 Y + スケール後の高さ)
				statBottomY = drawPos.y + (textRegion.h * numberScaleY);
			}

			// --- 4. ステージ結果画像（右下） ---
			if (m_stageResultTexture)
			{
				// 元のチャットボックスの幅(400)と右マージン(20)を参考に、画像サイズと位置を決定
				const double imageWidth = Scene::Width() * (400.0 / baseWidth);
				const double scaledHeight = m_stageResultTexture.height() * (imageWidth / m_stageResultTexture.width()); // アスペクト比維持

				// 「右」に配置
				const double marginRight = Scene::Width() * (20.0 / baseWidth); // 右端から20px (相対)
				const double drawX = Scene::Width() - imageWidth - marginRight;

				// Y座標の計算を、統計情報の下端 (statBottomY) 基準に変更
				// 画像の下端を statBottomY に合わせる
				const double drawY = statBottomY - scaledHeight;

				m_stageResultTexture.resized(imageWidth).draw(drawX, drawY);
			}


			// --- 5. ★カスタムボタンの描画 (アニメーション対応) ---
			const ColorF hoverColor = ColorF(244.0 / 255.0, 49.0 / 255.0, 89.0 / 255.0, 0.5);
			const ColorF defaultColor = ColorF(0.0, 0.0);

			// 1. ボタンの背景は常に透明で描画
			m_retryButton.draw(defaultColor);
			m_mapButton.draw(defaultColor);
			m_nextStageButton.draw(defaultColor);

			// 2. ホバー時の「帯」をアニメーション幅で描画 (相対座標)
			const double barHeight = m_retryButton.h * 0.4;
			const double barYOffset = Scene::Height() * (15.0 / baseHeight);

			// --- RETRY ボタン 帯 ---
			if (m_retryBarWidth > 0)
			{
				const double barY = m_retryButton.center().y - (barHeight / 2.0) + barYOffset;
				RectF(m_retryButton.x, barY, m_retryBarWidth, barHeight).draw(hoverColor);
			}

			// --- MAP ボタン 帯 ---
			if (m_mapBarWidth > 0)
			{
				const double barY = m_mapButton.center().y - (barHeight / 2.0) + barYOffset;
				RectF(m_mapButton.x, barY, m_mapBarWidth, barHeight).draw(hoverColor);
			}

			// --- NEXT STAGE ボタン 帯 ---
			if (m_nextStageBarWidth > 0)
			{
				const double barY = m_nextStageButton.center().y - (barHeight / 2.0) + barYOffset;
				RectF(m_nextStageButton.x, barY, m_nextStageBarWidth, barHeight).draw(hoverColor);
			}

			// 3. テキストを一番上に描画 (影も相対座標)
			const Vec2 shadowOffset = Vec2(Scene::Width() * (2.0 / baseWidth), Scene::Height() * (5.0 / baseHeight));
			const ColorF shadowColor = ColorF(0, 0, 0, 0.5);

			// RETRY ボタン
			m_buttonFont(U"RETRY").drawAt(m_retryButton.center() + shadowOffset, shadowColor);
			m_buttonFont(U"RETRY").drawAt(m_retryButton.center(), Palette::White);

			// MAP ボタン
			m_buttonFont(U"MAP").drawAt(m_mapButton.center() + shadowOffset, shadowColor);
			m_buttonFont(U"MAP").drawAt(m_mapButton.center(), Palette::White);

			// NEXT STAGE ボタン
			m_buttonFont(U"NEXT STAGE").drawAt(m_nextStageButton.center() + shadowOffset, shadowColor);
			m_buttonFont(U"NEXT STAGE").drawAt(m_nextStageButton.center(), Palette::White);

			if (Jam::Foundation::CoreManager::Instance().getPause())
			{
				Jam::Presentation::SettingManager::Instance().draw();
				return;
			}
		}

		void drawFadeIn(double t) const override
		{
			draw();
			Jam::Presentation::Scenes::TransitionManager::Instance().rec.drawFadeIn(t);
		}

		void drawFadeOut(double t) const override
		{
			draw();
			Jam::Presentation::Scenes::TransitionManager::Instance().rec.drawFadeOut(t);
		}
	};
}
