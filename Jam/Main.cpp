#include <Siv3D.hpp> // Siv3D v0.6.16
#include "src/Presentation/Scenes/TitleScene.h"
#include "src/Presentation/Scenes/GameScene.h"
#include "src/Presentation/Scenes/SelectScene.h"
#include "src/Presentation/Scenes/StoryScene.h"
#include "src/Foundation/CoreManager.h"
using App = SceneManager<String>;

void Main()
{
#ifdef _DEBUG
	Window::Resize(1280, 720);
#else
	Window::SetFullscreen(true);
#endif


	// =====================
	//  CoreManager 初期化
	// =====================
	auto& core = Jam::Foundation::CoreManager::Instance();
	core.stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;  // デフォルトステージ
	core.audioSetting.masterVolume = 1.0;
	core.audioSetting.bgmVolume = 0.7;
	core.audioSetting.seVolume = 1.0;

	App manager;

	manager.add<Jam::Presentation::Scenes::GameScene>(
		Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::InGame));
	manager.add<Jam::Presentation::Scenes::TitleScene>(
		Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::Title));
	manager.add<Jam::Presentation::Scenes::SelectScene>(
	Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::Select));
	manager.add<Jam::Presentation::Scenes::StoryScene>(
	Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::Story));

	// GameSceneから開始するように明示的に指定
	manager.init(Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::InGame));

	while (System::Update())
	{
		// シーン更新（falseなら終了）
		if (!manager.update())
			break;
	}
}
