#pragma once
#include <Siv3D.hpp>
#include "TitleScene.h"
#include "../StoryManager.h"
#include "../../Foundation/CoreManager.h"
#include "TransitionManager.h"

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
			: IScene{ init }, skipEmoji{ U"▶️"_emoji }
		{
			auto& core = Jam::Foundation::CoreManager::Instance();
			String stageName = Jam::Foundation::CoreManager::stageNameToString(core.stageInfo.stageName);

			String basePath = U"../Assets/Story/";

			// 立ち絵マップ作成（キャラクター名 "Player" などで分ける）
			HashTable<Speaker, HashTable<Jam::Domain::Portrait, FilePath>> portraits;

			portraits[Speaker::Player][Jam::Domain::Portrait::Normal] = basePath + U"Portrait/Player/player_normal.png";
			portraits[Speaker::Player][Jam::Domain::Portrait::Surprised] = basePath + U"Portrait/Player/player_surprised.png";
			portraits[Speaker::Player][Jam::Domain::Portrait::Suspicious] = basePath + U"Portrait/Player/player_suspicious.png";
			portraits[Speaker::Player][Jam::Domain::Portrait::Smiling] = basePath + U"Portrait/Player/player_smiling.png";
			portraits[Speaker::Player][Jam::Domain::Portrait::Conversation] = basePath + U"Portrait/Player/player_conversation.png";

			portraits[Speaker::Owner][Jam::Domain::Portrait::Normal] = basePath + U"Portrait/Owner/owner_normal.png";
			portraits[Speaker::Owner][Jam::Domain::Portrait::Surprised] = basePath + U"Portrait/Owner/owner_surprised.png";
			portraits[Speaker::Owner][Jam::Domain::Portrait::Suspicious] = basePath + U"Portrait/Owner/owner_suspicious.png";
			portraits[Speaker::Owner][Jam::Domain::Portrait::Smiling] = basePath + U"Portrait/Owner/owner_smiling.png";
			portraits[Speaker::Owner][Jam::Domain::Portrait::Conversation] = basePath + U"Portrait/Owner/owner_conversation.png";

			portraits[Speaker::Riska][Jam::Domain::Portrait::Normal] = basePath + U"Portrait/Riska/riska_normal.png";
			portraits[Speaker::Riska][Jam::Domain::Portrait::Surprised] = basePath + U"Portrait/Riska/riska_surprised.png";
			portraits[Speaker::Riska][Jam::Domain::Portrait::Suspicious] = basePath + U"Portrait/Riska/riska_suspicious.png";
			portraits[Speaker::Riska][Jam::Domain::Portrait::Smiling] = basePath + U"Portrait/Riska/riska_smiling.png";
			portraits[Speaker::Riska][Jam::Domain::Portrait::Conversation] = basePath + U"Portrait/Riska/riska_conversation.png";

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
			storyManager.update(Scene::DeltaTime());

			// スペースキーでゲーム開始
			if (MouseL.down())
			{
				if (storyManager.isEnd())
				{
					::TransitionManager::Instance().rec.init(30);
					changeScene(ToSceneString(SceneName::InGame), 1.0s); 
					//changeScene(ToSceneString(SceneName::InGame));
					return;
				}
				storyManager.next();
			}

			// クリック判定のみ
			if (Rect{ Scene::Width() - 150, 20, 80, 80 }.leftClicked())
			{
				::TransitionManager::Instance().rec.init(30);
				changeScene(ToSceneString(SceneName::InGame), 1.0s);
				//changeScene(ToSceneString(SceneName::InGame));
			}
		}

		// 描画処理
		void draw() const override
		{
			storyManager.draw();

			// StoryManagerの上にボタンを描画
			const Rect rect{ Scene::Width() - 150, 20, 80, 80 };
			const RoundRect roundRect = rect.rounded(6);
			skipEmoji.scaled(0.7).drawAt((rect.x + 50), rect.center().y);
		}
		void drawFadeIn(double t) const override
		{
			// 1. シーンを通常通り描画
			draw();

			// 2. トランジション（フェードイン）を上から描画
			//    t が 0.0 -> 1.0 になるにつれて、RectSlideが画面外に消えていく
			::TransitionManager::Instance().rec.drawFadeIn(t);
		}

		// シーンがフェードアウトする（消える）ときの描画
		void drawFadeOut(double t) const override
		{
			// 1. シーンを通常通り描画
			draw();

			// 2. トランジション（フェードアウト）を上から描画
			//    t が 0.0 -> 1.0 になるにつれて、RectSlideが画面を覆っていく
			::TransitionManager::Instance().rec.drawFadeOut(t);
		}

		bool button(const Rect& rect)
		{
			const RoundRect roundRect = rect.rounded(6);
			roundRect
				.drawShadow(Vec2{ 2, 2 }, 12, 0)
				.draw(ColorF{ 0.9, 0.8, 0.6 });

			// 枠を描く
			rect.stretched(-3).rounded(3)
				.drawFrame(2, ColorF{ 0.4, 0.3, 0.2 });
			// ボタンが押されたら true を返す
			return rect.leftClicked();
		}

	};
}
