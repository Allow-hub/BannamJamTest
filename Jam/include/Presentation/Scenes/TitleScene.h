#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14
#include "Presentation/Scenes/SceneName.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	class TitleScene : public App::Scene
	{
	private:
		s3d::Texture backgroundTexture;
		s3d::Texture logoTexture;
		s3d::Texture companyLogoTexture;

	public:
		TitleScene(const InitData& init);

		void update() override;
		void draw() const override;
		void drawFadeIn(double t) const override;
		void drawFadeOut(double t) const override;
	};
}
