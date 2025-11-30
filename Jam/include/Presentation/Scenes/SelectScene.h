#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14
#include "Presentation/Scenes/SceneName.h"
#include "Foundation/CoreManager.h"
#include "Presentation/AudioService.h"
#include "Presentation/ResourceManager.h"

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

		Texture m_worldBackgroundTexture;
		Texture m_stageBackgroundTexture;
		Texture m_worldButtonTexture_01;
		Texture m_worldButtonTexture_02;
		Texture m_worldButtonTexture_03;

		Texture m_stageButtonTexture_01;
		Texture m_stageButtonTexture_02;
		Texture m_stageButtonTexture_03;

		Texture m_backButtonTexture;

	public:
		SelectScene(const InitData& init);

		void update() override;
		void draw() const override;
		void drawFadeIn(double t) const override;
		void drawFadeOut(double t) const override;

	private:
		// --- ワールド選択 ---
		void updateWorldSelect();
		void drawWorldSelect() const;

		// --- ステージ選択 ---
		void updateStageSelect();
		void drawStageSelect() const;

		// --- 共通 ---
		void drawButton(const s3d::RectF& rect, const s3d::String& text, bool mouseOver, const s3d::Texture& buttonTexture, const s3d::String& fontName = U"SelectMenuFont") const;
	};
}
