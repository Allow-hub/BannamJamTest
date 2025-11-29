#pragma once
#include <Siv3D.hpp>
#include "Presentation/Scenes/SceneName.h"
#include "Foundation/CoreManager.h"
#include "Presentation/AudioService.h"
#include "Presentation/StoryManager.h"
#include "Presentation/ResourceManager.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	class StoryScene : public App::Scene
	{
	private:
		StoryManager storyManager;
		Texture skipEmoji;

	public:
		StoryScene(const InitData& init);

		void update() override;
		void draw() const override;
		void drawFadeIn(double t) const override;
		void drawFadeOut(double t) const override;

	private:
		void playStoryBGM();
		void initStory();
		//立ち絵の初期化
		void initPortraits(const String& basePath, HashTable<Speaker, HashTable<Jam::Domain::Portrait, FilePath>>& portraits);

		// ストーリー終了処理
		void handleStoryEnd();

		// スキップ処理
		void handleSkip();

		// スキップボタン描画
		void drawSkipButton() const;
	};
}
