#include <Siv3D.hpp> // Siv3D v0.6.16
#include "src/Presentation/Scenes/TitleScene.h"
#include "src/Presentation/Scenes/GameScene.h"

using App = SceneManager<String>;

void Main()
{
#ifdef _DEBUG
	// デバッグ時はウィンドウモード
	Window::Resize(1280, 720);
#else
	// リリース時は全画面
	Window::SetFullscreen(true);
#endif

	// シーンマネージャーを作成
	App manager;

	//デバッグ用に最初からInGameで
	manager.add<Jam::Scenes::GameScene>(Jam::Scenes::ToSceneString(Jam::Scenes::SceneName::InGame));

	// タイトルシーンを登録
	manager.add<Jam::Scenes::TitleScene>(Jam::Scenes::ToSceneString(Jam::Scenes::SceneName::Title));

	while (System::Update())
	{
		// 現在のシーンを実行する
		// シーンに実装した .update() と .draw() が実行される
		if (not manager.update())
		{
			break;
		}
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
