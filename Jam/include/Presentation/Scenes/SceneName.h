#pragma once
#include <Siv3D.hpp>

namespace Jam::Presentation::Scenes
{
	// シーン名の定義
	enum class SceneName {
		Title,
		Select,
		Story,
		InGame,
		Result,
		Editor
	};

	// シーン名を文字列に変換する関数
	inline s3d::String ToSceneString(SceneName name)
	{
		switch (name) {
		case SceneName::Title:   return U"Title";
		case SceneName::Select:  return U"Select";
		case SceneName::Story:   return U"Story";
		case SceneName::InGame:  return U"InGame";
		case SceneName::Result:  return U"Result";
		case SceneName::Editor:  return U"StageEditor";
		default:                 return U"";
		}
	}
}
