#pragma once
#include <Siv3D.hpp>
#include "TitleScene.h"
#include "../StoryManager.h"
#include "../../Foundation/CoreManager.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	class StoryScene : public App::Scene
	{
	private:
		StoryManager storyManager; // メンバ変数にする

	public:
		// コンストラクタ
		StoryScene(const InitData& init)
			: IScene{ init }
		{
			auto& core = Jam::Foundation::CoreManager::Instance();
			String stageName = Jam::Foundation::CoreManager::stageNameToString(core.stageInfo.stageName);

			String basePath = U"../Assets/Story/";

			// 立ち絵マップ作成（キャラクター名 "Player" などで分ける）
			HashTable<String, HashTable<Jam::Domain::Portrait, FilePath>> portraits;

			portraits[U"Player"][Jam::Domain::Portrait::Normal] = basePath + U"Portrait/Player/player_normal.png";
			portraits[U"Player"][Jam::Domain::Portrait::Surprised] = basePath + U"Portrait/Player/player_surprised.png";
			portraits[U"Player"][Jam::Domain::Portrait::Suspicious] = basePath + U"Portrait/Player/player_suspicious.png";
			portraits[U"Player"][Jam::Domain::Portrait::Smiling] = basePath + U"Portrait/Player/player_smiling.png";
			portraits[U"Player"][Jam::Domain::Portrait::Conversation] = basePath + U"Portrait/Player/player_conversation.png";

			portraits[U"Owner"][Jam::Domain::Portrait::Normal] = basePath + U"Portrait/Owner/owner_normal.png";
			portraits[U"Owner"][Jam::Domain::Portrait::Surprised] = basePath + U"Portrait/Owner/owner_surprised.png";
			portraits[U"Owner"][Jam::Domain::Portrait::Suspicious] = basePath + U"Portrait/Owner/owner_suspicious.png";
			portraits[U"Owner"][Jam::Domain::Portrait::Smiling] = basePath + U"Portrait/Owner/owner_smiling.png";
			portraits[U"Owner"][Jam::Domain::Portrait::Conversation] = basePath + U"Portrait/Owner/owner_conversation.png";

			portraits[U"Riska"][Jam::Domain::Portrait::Normal] = basePath + U"Portrait/Riska/riska_normal.png";
			portraits[U"Riska"][Jam::Domain::Portrait::Surprised] = basePath + U"Portrait/Riska/riska_surprised.png";
			portraits[U"Riska"][Jam::Domain::Portrait::Suspicious] = basePath + U"Portrait/Riska/riska_suspicious.png";
			portraits[U"Riska"][Jam::Domain::Portrait::Smiling] = basePath + U"Portrait/Riska/riska_smiling.png";
			portraits[U"Riska"][Jam::Domain::Portrait::Conversation] = basePath + U"Portrait/Riska/riska_conversation.png";

			// StoryManager初期化
			bool ok = storyManager.init(
				basePath + stageName + U".csv",
				portraits,
				basePath + U"background_" + stageName + U".png"
			);

			if (!ok)
			{
				Print << U"StoryManager initialization failed";
			}
		}

		// 更新処理
		void update() override
		{
			// スペースキーでゲーム開始
			if (KeySpace.down())
			{
				changeScene(ToSceneString(SceneName::InGame));
			}
		}

		// 描画処理
		void draw() const override
		{
			storyManager.draw();
		}
	};
}
