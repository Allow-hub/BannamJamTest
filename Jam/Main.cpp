#include <Siv3D.hpp> // Siv3D v0.6.16
#include "src/Presentation/Scenes/TitleScene.h"
#include "src/Presentation/Scenes/GameScene.h"
#include "src/Presentation/Scenes/SelectScene.h"
#include "src/Presentation/Scenes/StoryScene.h"
#include "src/Foundation/CoreManager.h"
#include "src/Foundation/CoroutineUtil.h"
#include "src/Presentation/AudioService.h"
#include "src/Presentation/Scenes/ResultScene.h"
#include "src/Presentation/Scenes/TransitionManager.h"
#include "src/Presentation/SettingManager.h"

using App = SceneManager<String>;

void Main()
{
#ifdef _DEBUG
	Window::Resize(1280, 720);
#else
	Window::SetFullscreen(true);
#endif
	Window::SetTitle(U"シン層シン理 地雷ちゃん");

	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
	// =====================
	//  CoreManager 初期化
	// =====================
	auto& core = Jam::Foundation::CoreManager::Instance();
	core.stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;  // デフォルトステージ
	core.reset();
	core.audioSetting.masterVolume = 1.0;
	core.audioSetting.bgmVolume = 0.4;
	core.audioSetting.seVolume = 1.0;

	// --- 音声ロード ---
	auto& audioService = Jam::Presentation::AudioService::get();
	audioService.init();
	// === マスターボリューム設定 ===
	audioService.setMasterVolume(core.audioSetting.masterVolume);

	// === BGMボリューム設定 ===
	audioService.setBGMVolume(core.audioSetting.bgmVolume); 

	// === SEボリューム設定 ===
	audioService.setSEVolume(core.audioSetting.seVolume); 

	Jam::Presentation::Scenes::TransitionManager::Instance().rec.init(30);

	App manager;
	auto& settingMgr = Jam::Presentation::SettingManager::Instance();
	settingMgr.init();
	settingMgr.setSceneManager(&manager);

	manager.add<Jam::Presentation::Scenes::TitleScene>(
		Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::Title));
	manager.add<Jam::Presentation::Scenes::GameScene>(
		Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::InGame));
	manager.add<Jam::Presentation::Scenes::SelectScene>(
	Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::Select));
	manager.add<Jam::Presentation::Scenes::StoryScene>(
	Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::Story));
	manager.add<Jam::Presentation::Scenes::ResultScene>(
		Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::Result));

	// GameSceneから開始するように明示的に指定
	manager.init(Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::InGame));

	while (System::Update())
	{
		Jam::Util::CoroutineUtil::Update(Scene::DeltaTime());
		// シーン更新（falseなら終了）
		if (!manager.update())
			break;
	}
}
