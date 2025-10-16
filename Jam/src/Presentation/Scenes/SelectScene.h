#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14
#include "TitleScene.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;
	class SelectScene : public App::Scene

	{
	public:
		// コンストラクタ
		SelectScene(const InitData& init)
			: IScene{ init } {
		}

		// 更新処理
		void update() override
		{
			// スペースキーでゲーム開始
			if (KeySpace.down())
			{
				// InGame へ遷移
				changeScene(ToSceneString(SceneName::Story));
			}
		}

		// 描画処理
		void draw() const override
		{
		}
	};
}
