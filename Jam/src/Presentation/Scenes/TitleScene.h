#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14
#include "../../Presentation/AudioService.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	// シーン名の定義
	enum class SceneName {
		Title,
		Select,
		Story,
		InGame,
		Result
	};

	// シーン名を文字列に変換するヘルパー関数
	inline String ToSceneString(SceneName name) {
		switch (name) {
		case SceneName::Title:   return U"Title";
		case SceneName::Select:  return U"Select";
		case SceneName::Story:   return U"Story";
		case SceneName::InGame:  return U"InGame";
		case SceneName::Result:  return U"Result";
		default:                 return U"";
		}
	}
	class TitleScene : public App::Scene
	{
	public:
		// コンストラクタ
		TitleScene(const InitData& init)
			: IScene{ init }
		{
			Jam::Presentation::AudioService::get().play(Jam::Presentation::AudioService::Sound::BGM_Title, true);
		}
		// 更新処理
		void update() override
		{
			const auto& inputs = Keyboard::GetAllInputs();

			// 何かキーが押されたかチェック
			for (const auto& input : inputs)
			{
				if (input.down())
				{
					Print << U"Key pressed!";
					break;
				}
			}
		}

		// 描画処理
		void draw() const override
		{

		}
	};
}
