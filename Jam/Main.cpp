#include <Siv3D.hpp> // Siv3D v0.6.16
#include "src/Presentation/Scenes/TitleScene.h"
#include "src/Presentation/Scenes/GameScene.h"

using App = SceneManager<String>;

void Main()
{
#ifdef _DEBUG
	Window::Resize(1280, 720);
#else
	Window::SetFullscreen(true);
#endif

	App manager;


	manager.add<Jam::Presentation::Scenes::GameScene>(
		Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::InGame));
	manager.add<Jam::Presentation::Scenes::TitleScene>(
		Jam::Presentation::Scenes::ToSceneString(Jam::Presentation::Scenes::SceneName::Title));

	while (System::Update())
	{
		// シーン更新（falseなら終了）
		if (!manager.update())
			break;
	}
}
