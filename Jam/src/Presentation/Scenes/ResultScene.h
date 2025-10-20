#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14
#include "TitleScene.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;
	class ResultScene : public App::Scene
	{
	public:
		// コンストラクタ
		ResultScene(const InitData& init)
			: IScene{ init }
		{
		}

		// 更新処理
		void update() override
		{
		}

		// 描画処理
		void draw() const override
		{
		}
	};
}
