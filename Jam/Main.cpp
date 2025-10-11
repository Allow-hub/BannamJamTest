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

	// 物理ワールド
	P2World world;
	constexpr double StepTime = (1.0 / 200.0);
	double accumulatedTime = 0.0;

	while (System::Update())
	{
		// 経過時間を蓄積
		accumulatedTime += Scene::DeltaTime();

		// 固定ステップで物理更新
		while (accumulatedTime >= StepTime)
		{
			world.update(StepTime);
			accumulatedTime -= StepTime;
		}

		// シーン更新（falseなら終了）
		if (!manager.update())
			break;
	}
}
