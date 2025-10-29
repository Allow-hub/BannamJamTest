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
		unsigned int m_time;

		// --- フォント ---
		Font m_titleFont;
		Font m_statLabelFont;
		Font m_statValueFont;
		Font m_buttonFont;

		// --- テクスチャ ---
		Texture m_background;
		Texture m_chatBg;
		Texture m_iconRisuka;
		Texture m_iconYumemi;
		Texture m_titleMemory;
		Texture m_titleTime;
		Texture m_titleKill;
		Texture m_titleGameOver;
		Texture m_titleClear;

		// --- カスタムボタン用の Rect ---
		Rect m_retryButton;
		Rect m_mapButton;
		Rect m_nextStageButton;

		double m_retryBarWidth = 0.0;
		double m_mapBarWidth = 0.0;
		double m_nextStageBarWidth = 0.0;

		const Color m_statColor = Color(164, 179, 255);

	public:
		// コンストラクタ
		ResultScene(const InitData& init)
			: IScene{ init }
		{
			auto& core = Jam::Foundation::CoreManager::Instance();
			m_isClear = core.getClear();
			m_flagmentAmount = core.getFlagment();
			m_maxFlagment = core.getMaxFlagment();
			m_time = static_cast<unsigned int>(core.getTimer());

			// --- フォントの読み込み ---
			m_titleFont = Font(100, Typeface::Heavy);
			m_statLabelFont = Font(30, Typeface::Bold);

			m_statValueFont = Font(130, U"../Assets/Font/PixelMplus12-Bold.ttf");
			if (not m_statValueFont)
			{
				Print(U"Error: StatValueFont 'PixelMplus12-Bold.ttf' not found. Using default.");
				m_statValueFont = Font(130, Typeface::Bold);
			}

			const int32 mainFontSize = static_cast<int32>(Scene::Height() * 0.07);
			m_buttonFont = Font(mainFontSize, U"../Assets/Font/PixelMplus12-Bold.ttf");
			//フォールバック処理を追加
			if (not m_buttonFont)
			{
				Print(U"Error: ButtonFont 'PixelMplus12-Bold.ttf' not found. Using default.");
				m_buttonFont = Font(mainFontSize, Typeface::Bold);
			}

			// カスタムボタンの座標を初期化
			const double buttonWidth = 400;
			const double buttonX = Scene::Center().x - (buttonWidth / 2);
			const double buttonHeight = 80;
			const double buttonYBase = 650;
			const double buttonMargin = 90;

			m_retryButton = Rect(static_cast<int32>(buttonX), static_cast<int32>(buttonYBase), static_cast<int32>(buttonWidth), static_cast<int32>(buttonHeight));
			m_mapButton = Rect(static_cast<int32>(buttonX), static_cast<int32>(buttonYBase + buttonMargin), static_cast<int32>(buttonWidth), static_cast<int32>(buttonHeight));
			m_nextStageButton = Rect(static_cast<int32>(buttonX), static_cast<int32>(buttonYBase + buttonMargin * 2), static_cast<int32>(buttonWidth), static_cast<int32>(buttonHeight));


			// === テクスチャ読み込み ===
			const FilePath assetRoot = FileSystem::InitialDirectory() + U"../Assets/Result/";

			m_background = Texture(assetRoot + U"GAME_OVER_screen.png");
			m_chatBg = Texture(assetRoot + U"CLEAR_backscreen.png");
			m_iconRisuka = Texture(assetRoot + U"ICON-RISUKA.png");
			m_iconYumemi = Texture(assetRoot + U"ICON-YUMEMI.png");
			m_titleGameOver = Texture(assetRoot + U"Result_Game_Over.png");
			m_titleClear = Texture(assetRoot + U"Result_Crear.png");
			m_titleMemory = Texture(assetRoot + U"result_title-MEMORY.png");
			m_titleTime = Texture(assetRoot + U"result_TIME.png");
			m_titleKill = Texture(assetRoot + U"result_KILL.png");

			// === フォールバック ===
			if (not m_background) { m_background = Texture(U"example/city.png"); }
			if (not m_chatBg) { m_chatBg = Texture(U"example/white_rect.png"); }
			if (not m_iconRisuka) { m_iconRisuka = Texture(U"example/siv3d-kun.png"); }
			if (not m_iconYumemi) { m_iconYumemi = Texture(U"example/siv3d-kun.png"); }
			if (not m_titleGameOver) { Print(U"Error: Result_Game_Over.png not found!"); }
			if (not m_titleClear) { Print(U"Error: Result_Crear.png not found!"); }
			if (not m_titleMemory) { Print(U"Error: result_title-MEMORY.png not found!"); }
			if (not m_titleTime) { Print(U"Error: result_TIME.png not found!"); }
			if (not m_titleKill) { Print(U"Error: result_KILL.png not found!"); }
		}


		// 更新処理
		void update() override
		{
			// 1. RETRY ボタン
			if (m_retryButton.leftClicked())
			{
				changeScene(ToSceneString(SceneName::Title));
			}

			// 2. MAP ボタン
			if (m_mapButton.leftClicked())
			{
				changeScene(ToSceneString(SceneName::Select));
			}

			// 3. NEXT STAGE ボタン
			if (m_nextStageButton.leftClicked())
			{
				if (m_isClear)
				{
					Print(U"Loading Next Stage...");
					changeScene(ToSceneString(SceneName::Select));
				}
				else
				{
					changeScene(ToSceneString(SceneName::Select));
				}
			}
			// 帯の最大幅（ボタンの横幅と同じ）
			const double targetWidth = m_retryButton.w;
			// 帯が伸びる速度
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

		}

		// 描画処理 
		void draw() const override
		{
			// 1. 背景の描画
			m_background.scaled(Max(Scene::Size().x / (double)m_background.width(), Scene::Size().y / (double)m_background.height()))
				.draw(0, 0, ColorF(0.8));

			// 2. タイトルの描画
			const Vec2 titleCenter = Scene::Center().movedBy(0, -200);

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
			const double leftStatX = 50;
			const double labelYOffset = -20;
			const double statYOffset = 200;
			const double blockSpacing = 170;
			const double numberVerticalAdjustment = 20;

			const double numberScaleY = 0.7;
			const double numberScaleX = 1.0;
			const Vec2 numberScale = Vec2(numberScaleX, numberScaleY);

			double currentY = statYOffset + 250;

			// FRAGMENTS OF MEMORY
			if (m_titleMemory) m_titleMemory.draw(leftStatX, currentY + labelYOffset);

			{
				const Vec2 drawPos = Vec2(leftStatX + 20, currentY + labelYOffset + numberVerticalAdjustment + m_titleMemory.height());
				const Mat3x2 jam_matrix = Mat3x2::Scale(numberScale) * Mat3x2::Translate(drawPos);

				const Transformer2D jam_scaler(jam_matrix);

				m_statValueFont(U"{}/{}"_fmt(m_flagmentAmount, m_maxFlagment)).draw(Vec2(0, 0), m_statColor);
			}

			// 次のブロックのY座標
			currentY += blockSpacing;

			// TIME
			if (m_titleTime) m_titleTime.draw(leftStatX, currentY + labelYOffset);
			{
				const Vec2 drawPos = Vec2(leftStatX + 20, currentY + labelYOffset + numberVerticalAdjustment + m_titleTime.height());
				const Mat3x2 jam_matrix = Mat3x2::Scale(numberScale) * Mat3x2::Translate(drawPos);

				const Transformer2D jam_scaler(jam_matrix);

				m_statValueFont(FormatTime(SecondsF(m_time), U"mm:ss")).draw(Vec2(0, 0), m_statColor);
			}

			// 次のブロックのY座標
			currentY += blockSpacing;

			// KILL RACIO
			const int killRacio_demo = 0;
			if (m_titleKill) m_titleKill.draw(leftStatX, currentY + labelYOffset);
			{
				const Vec2 drawPos = Vec2(leftStatX + 20, currentY + labelYOffset + numberVerticalAdjustment + m_titleKill.height());
				const Mat3x2 jam_matrix = Mat3x2::Scale(numberScale) * Mat3x2::Translate(drawPos);

				const Transformer2D jam_scaler(jam_matrix);

				m_statValueFont(Pad(killRacio_demo, { 3, U'0' })).draw(Vec2(0, 0), m_statColor);
			}

			// 4. チャットボックス（右側）
			const double rightChatX = Scene::Width() - 420;
			const double rightChatY = 250;
			const double rightChatWidth = 400;
			const double rightChatHeight = 400;

			m_chatBg.resized(rightChatWidth, rightChatHeight).draw(rightChatX, rightChatY, ColorF(1.0, 0.8));
			m_iconRisuka.resized(60, 60).draw(rightChatX + 10, rightChatY + 20);
			RectF(rightChatX + 80, rightChatY + 20, 300, 80).draw(ColorF(0.2)).drawFrame(1, Palette::White);
			m_iconYumemi.resized(60, 60).draw(rightChatX + 10, rightChatY + 120);
			RectF(rightChatX + 80, rightChatY + 120, 300, 80).draw(ColorF(0.2)).drawFrame(1, Palette::White);

			// --- 5. ★カスタムボタンの描画 (アニメーション対応) ---
			const ColorF hoverColor = ColorF(244.0 / 255.0, 49.0 / 255.0, 89.0 / 255.0, 0.5);
			const ColorF defaultColor = ColorF(0.0, 0.0);

			// 1. ボタンの背景は常に透明で描画
			m_retryButton.draw(defaultColor);
			m_mapButton.draw(defaultColor);
			m_nextStageButton.draw(defaultColor);

			// 2. ホバー時の「帯」をアニメーション幅で描画
			const double barHeight = m_retryButton.h * 0.4; // 帯の高さ (40%)
			const double barYOffset = 15.0;

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

			// 3. テキストを一番上に描画
			const Vec2 shadowOffset = Vec2(2, 5);
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
