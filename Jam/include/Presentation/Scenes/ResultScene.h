#pragma once
#include <Siv3D.hpp>
#include "SceneName.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	class ResultScene : public App::Scene
	{
	private:
		bool m_isClear;
		int m_flagmentAmount;
		int m_maxFlagment;
		int m_defeatedCount;
		unsigned int m_time;

		Font m_titleFont;
		Font m_statLabelFont;
		Font m_statValueFont;
		Font m_buttonFont;

		Texture m_background;
		Texture m_titleMemory;
		Texture m_titleTime;
		Texture m_titleKill;
		Texture m_titleGameOver;
		Texture m_titleClear;
		Texture m_stageResultTexture;

		RectF m_retryButton;
		RectF m_mapButton;
		RectF m_nextStageButton;

		double m_retryBarWidth = 0.0;
		double m_mapBarWidth = 0.0;
		double m_nextStageBarWidth = 0.0;

		const Color m_statColor = Color(164, 179, 255);

	public:
		ResultScene(const InitData& init);

		void update() override;
		void draw() const override;

		void drawFadeIn(double t) const override;
		void drawFadeOut(double t) const override;
	};
}
