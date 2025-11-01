#pragma once
#include <Siv3D.hpp>
#include "TitleScene.h"
#include "../StoryManager.h"
#include "../../Foundation/CoreManager.h"
#include "TransitionManager.h"
#include "../../Presentation/ResourceManager.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	class StoryScene : public App::Scene
	{
	private:
		StoryManager storyManager;
		Texture skipEmoji;

	public:
		// コンストラクタ
		StoryScene(const InitData& init)
			: IScene{ init }
			, skipEmoji{ U"▶️"_emoji }
		{
			initStory();
		}

		// 更新処理
		void update() override
		{
			storyManager.update(Scene::DeltaTime());

			// 左クリック・スペースで進行
			if (MouseL.down())
			{
				if (storyManager.isEnd())
				{
					Jam::Presentation::Scenes::TransitionManager::Instance().rec.init(30);
					handleStoryEnd();
					return;
				}

				// ストーリーを次へ
				storyManager.next();
			}

			// 右上のボタンクリックでスキップ
			const Rect skipRect{ Scene::Width() - 150, 20, 80, 80 };
			if (skipRect.leftClicked())
			{
				handleSkip();
			}
			if(KeyEscape.down())
			{
				Jam::Foundation::CoreManager::Instance().setPause(!Jam::Foundation::CoreManager::Instance().getPause());
			}	
			if (Jam::Foundation::CoreManager::Instance().getPause())
			{
				Jam::Presentation::SettingManager::Instance().update();
				return;
			}
		}

		// 描画処理
		void draw() const override
		{
			storyManager.draw();
			drawSkipButton();
			if (Jam::Foundation::CoreManager::Instance().getPause())
			{
				Jam::Presentation::SettingManager::Instance().draw();
				return;
			}
		}

		void drawFadeIn(double t) const override
		{
			draw();
			Jam::Presentation::Scenes::TransitionManager::Instance().rec.drawFadeIn(t);
		}

		void drawFadeOut(double t) const override
		{
			draw();
			Jam::Presentation::Scenes::TransitionManager::Instance().rec.drawFadeOut(t);
		}

	private:
		// ストーリー初期化
		void initStory()
		{
			ResourceManager::initPlayerWalk();
			ResourceManager::loadGroup(ResourceGroup::PlayerWalk);


			auto& core = Jam::Foundation::CoreManager::Instance();
			String stageName = Jam::Foundation::CoreManager::stageNameToString(core.stageInfo.stageName);
			String basePath = U"Assets/Story/";

			// 立ち絵マップ作成
			HashTable<Speaker, HashTable<Jam::Domain::Portrait, FilePath>> portraits;
			initPortraits(basePath, portraits);

			// ストーリーの開始ファイルを判定
			String storySuffix = core.getClear() ? U"_clear" : U"_start";
			//Print << basePath + stageName + storySuffix + U".csv";

			// StoryManager初期化
			bool ok = storyManager.init(
				basePath + stageName + storySuffix + U".csv",
				portraits,
				basePath + U"background_" + stageName + storySuffix + U".png"
			);

			if (!ok)
			{
				Print << U"StoryManager initialization failed";
			}
		}

		// 各キャラクターの立ち絵ファイルを登録
		void initPortraits(const String& basePath, HashTable<Speaker, HashTable<Jam::Domain::Portrait, FilePath>>& portraits)
		{
			// Player
			portraits[Speaker::Player][Jam::Domain::Portrait::Normal] = basePath + U"Portrait/Player/player_normal.png";
			portraits[Speaker::Player][Jam::Domain::Portrait::Surprised] = basePath + U"Portrait/Player/player_surprised.png";
			portraits[Speaker::Player][Jam::Domain::Portrait::Suspicious] = basePath + U"Portrait/Player/player_suspicious.png";
			portraits[Speaker::Player][Jam::Domain::Portrait::Smiling] = basePath + U"Portrait/Player/player_smiling.png";
			portraits[Speaker::Player][Jam::Domain::Portrait::Conversation] = basePath + U"Portrait/Player/player_conversation.png";

			// Owner
			portraits[Speaker::Owner][Jam::Domain::Portrait::Normal] = basePath + U"Portrait/Owner/owner_normal.png";
			portraits[Speaker::Owner][Jam::Domain::Portrait::Surprised] = basePath + U"Portrait/Owner/owner_surprised.png";
			portraits[Speaker::Owner][Jam::Domain::Portrait::Suspicious] = basePath + U"Portrait/Owner/owner_suspicious.png";
			portraits[Speaker::Owner][Jam::Domain::Portrait::Smiling] = basePath + U"Portrait/Owner/owner_smiling.png";
			portraits[Speaker::Owner][Jam::Domain::Portrait::Conversation] = basePath + U"Portrait/Owner/owner_conversation.png";

			// Riska
			portraits[Speaker::Riska][Jam::Domain::Portrait::Normal] = basePath + U"Portrait/Riska/riska_normal.png";
			portraits[Speaker::Riska][Jam::Domain::Portrait::Surprised] = basePath + U"Portrait/Riska/riska_surprised.png";
			portraits[Speaker::Riska][Jam::Domain::Portrait::Suspicious] = basePath + U"Portrait/Riska/riska_suspicious.png";
			portraits[Speaker::Riska][Jam::Domain::Portrait::Smiling] = basePath + U"Portrait/Riska/riska_smiling.png";
			portraits[Speaker::Riska][Jam::Domain::Portrait::Conversation] = basePath + U"Portrait/Riska/riska_conversation.png";
		}

		// ストーリー終了時の処理
		void handleStoryEnd()
		{
			auto& core = Jam::Foundation::CoreManager::Instance();

			if (core.getClear())
			{
				core.setClear(false);

				// ネクストステージ押下ならストーリー画面、そうでなければセレクト
				if (core.getNextStagePressed())
				{
					core.goToNextStage();
					changeScene(ToSceneString(SceneName::Story), 1.0s);
				}
				else
				{
					changeScene(ToSceneString(SceneName::Select), 1.0s);
				}
			}
			else
			{
				core.setClear(false);
				changeScene(ToSceneString(SceneName::InGame), 1.0s);
			}
		}

		// スキップボタン処理
		void handleSkip()
		{
			auto& core = Jam::Foundation::CoreManager::Instance();

			if (core.getClear())
			{
				core.setClear(false);

				// ネクストステージ押下ならストーリー画面、そうでなければセレクト
				if (core.getNextStagePressed())
				{
					core.goToNextStage();
					changeScene(ToSceneString(SceneName::Story), 1.0s);
				}
				else
				{
					changeScene(ToSceneString(SceneName::Select), 1.0s);
				}
			}
			else
			{
				core.setClear(false);
				changeScene(ToSceneString(SceneName::InGame), 1.0s);
			}
		}

		// スキップボタン描画
		void drawSkipButton() const
		{
			const Rect rect{ Scene::Width() - 150, 20, 80, 80 };
			const RoundRect roundRect = rect.rounded(6);
			skipEmoji.scaled(0.7).drawAt((rect.x + 50), rect.center().y);
		}
	};
}
