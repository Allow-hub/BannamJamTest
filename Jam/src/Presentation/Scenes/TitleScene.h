#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	// シーン名の定義
	enum class SceneName {
		Title,
		InGame,
	};

	// シーン名を文字列に変換するヘルパー関数
	inline String ToSceneString(SceneName name) {
		switch (name) {
		case SceneName::Title:   return U"Title";
		case SceneName::InGame:  return U"InGame";
		default:                 return U"";
		}
	}
	class TitleScene : public App::Scene
	{
	public:
		// コンストラクタ
		TitleScene(const InitData& init)
			: IScene{ init } {
		}

		// 更新処理
		void update() override
		{
			// スペースキーでゲーム開始
			if (KeySpace.down())
			{
				// InGame へ遷移
				changeScene(ToSceneString(SceneName::InGame));
			}
		}

		// 描画処理
		void draw() const override
		{
		}
	};
}
