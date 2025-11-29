#include "StoryScene.h"
#include "../SettingManager.h"
#include "../../Presentation/Scenes/Transition/TransitionManager.h"

namespace Jam::Presentation::Scenes
{
	StoryScene::StoryScene(const InitData& init)
		: IScene{ init }
		, skipEmoji{ U"▶️"_emoji }
	{
		playStoryBGM();
		initStory();
	}

	void StoryScene::update()
	{
		storyManager.update(Scene::DeltaTime());

		if (MouseL.down())
		{
			if (storyManager.isEnd())
			{
				handleStoryEnd();
				return;
			}
			storyManager.next();
		}

		const Rect skipRect{ Scene::Width() - 150, 20, 80, 80 };
		if (skipRect.leftClicked())
		{
			AudioService::get().playOneShot(AudioService::Sound::SE_Button, AudioService::VOLUME_BUTTON);
			handleSkip();
		}

		if (KeyEscape.down())
		{
			Jam::Foundation	::CoreManager::Instance().setPause(!Jam::Foundation::CoreManager::Instance().getPause());
		}

		if (Jam::Foundation::CoreManager::Instance().getPause())
		{
			Jam::Presentation::SettingManager::Instance().update();
			return;
		}
	}

	void StoryScene::draw() const
	{
		storyManager.draw();
		drawSkipButton();

		if (Jam::Foundation::CoreManager::Instance().getPause())
		{
			SettingManager::Instance().draw();
			return;
		}
	}

	void StoryScene::drawFadeIn(double t) const
	{
		draw();
		Jam::Presentation::Scenes::TransitionManager::Instance().drawFadeIn(
		Jam::Presentation::Scenes::TransitionType::RectSlide,
		t
		);
	}

	void StoryScene::drawFadeOut(double t) const
	{
		draw();
		Jam::Presentation::Scenes::TransitionManager::Instance().drawFadeOut(
		Jam::Presentation::Scenes::TransitionType::RectSlide,
		t
		);
	}

	void StoryScene::playStoryBGM()
	{
		auto& core = Jam::Foundation::CoreManager::Instance();
		AudioService::Sound storyBGM;

		switch (core.stageInfo.stageName)
		{
		case Jam::Foundation::StageName::Stage1_1:
			storyBGM = core.getClear() ? AudioService::Sound::BGM_Story2 : AudioService::Sound::BGM_Story1;
			break;
		case Jam::Foundation::StageName::Stage1_2:
		case Jam::Foundation::StageName::Stage1_3:
			storyBGM = AudioService::Sound::BGM_Story2;
			break;
		default:
			storyBGM = AudioService::Sound::BGM_Story1;
			break;
		}

		AudioService::get().play(storyBGM, true);
	}

	void StoryScene::initStory()
	{
		ResourceManager::initPlayerWalk();
		ResourceManager::loadGroup(ResourceGroup::PlayerWalk);

		auto& core = Jam::Foundation::CoreManager::Instance();
		String stageName = Jam::Foundation::CoreManager::stageNameToString(core.stageInfo.stageName);
		String basePath = U"Assets/Story/";

		HashTable<Speaker, HashTable<Jam::Domain::Portrait, FilePath>> portraits;
		initPortraits(basePath, portraits);

		String storySuffix = core.getClear() ? U"_clear" : U"_start";

		if (!storyManager.init(
			basePath + stageName + storySuffix + U".csv",
			portraits,
			basePath + U"background_" + stageName + storySuffix + U".png"
		)) {
			Print << U"StoryManager initialization failed";
		}
	}

	void StoryScene::initPortraits(const String& basePath, HashTable<Speaker, HashTable<Jam::Domain::Portrait, FilePath>>& portraits)
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

	void StoryScene::handleStoryEnd()
	{
		auto& core = Jam::Foundation::CoreManager::Instance();

		if (core.getClear())
		{
			core.setClear(false);

			// ネクストステージ押下状態ならストーリー画面、そうでなければセレクト
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

	void StoryScene::handleSkip()
	{
		handleStoryEnd();
	}

	void StoryScene::drawSkipButton() const
	{
		const Rect rect{ Scene::Width() - 150, 20, 80, 80 };
		skipEmoji.scaled(0.7).drawAt((rect.x + 50), rect.center().y);
	}
}
