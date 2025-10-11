#pragma once
#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include "TitleScene.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	class GameScene : public App::Scene
	{

	public:
		GameScene(const InitData& init)
			: IScene{ init }
		{
		}

		void update() override
		{
		}

		void draw() const override
		{
			Scene::SetBackground(ColorF{ 0.9, 0.9, 1.0 });
		}
	};
}
