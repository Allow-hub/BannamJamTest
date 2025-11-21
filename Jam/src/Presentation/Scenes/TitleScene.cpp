#include "TitleScene.h"
#include "../../Presentation/AudioService.h"
#include "../../Presentation/Scenes/Transition/TransitionManager.h"
#include "../../Presentation/ResourceManager.h"

namespace Jam::Presentation::Scenes
{
	TitleScene::TitleScene(const InitData& init)
		: IScene{ init }
	{
		ResourceManager::initPlayerIdle();
		ResourceManager::loadGroup(ResourceGroup::PlayerIdle);

		ResourceManager::initPlayerJump();
		ResourceManager::loadGroup(ResourceGroup::PlayerJump);

		backgroundTexture = Texture(Resource(U"Assets/Title/background.png"));
		logoTexture = Texture(Resource(U"Assets/Title/logo.png"));
		companyLogoTexture = Texture(Resource(U"Assets/Title/companyLogo.png"));

		Jam::Presentation::AudioService::get().play(Jam::Presentation::AudioService::Sound::BGM_Title, true);
	}

	void TitleScene::update()
	{
		const auto& inputs = Keyboard::GetAllInputs();

		for (const auto& input : inputs)
		{
			if (input.down())
			{
				changeScene(ToSceneString(SceneName::Select), 1.0);
				break;
			}
		}

		if (MouseL.down() || MouseR.down())
		{
			changeScene(ToSceneString(SceneName::Select), 1.0);
		}
	}

	void TitleScene::draw() const
	{
		const double w = Scene::Width();
		const double h = Scene::Height();

		backgroundTexture.scaled(w / backgroundTexture.width()).draw(0, 0);
		logoTexture.scaled(w / logoTexture.width() * 0.5).draw(0, h / 2.3);
		companyLogoTexture.scaled(w / companyLogoTexture.width() * 0.11).draw(w / 1.2, h / 1.2);
	}

	void TitleScene::drawFadeIn(double t) const
	{
		draw();
		TransitionManager::Instance().drawFadeIn(t);
	}

	void TitleScene::drawFadeOut(double t) const
	{
		draw();
		TransitionManager::Instance().drawFadeOut(t);
	}
}
