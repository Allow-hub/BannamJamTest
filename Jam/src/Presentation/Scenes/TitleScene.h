#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14
#include "../../Presentation/AudioService.h"
#include "TransitionManager.h"
#include "../../Presentation/ResourceManager.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	// シーン名の定義
	enum class SceneName {
		Title,
		Select,
		Story,
		InGame,
		Result
	};

	// シーン名を文字列に変換するヘルパー関数
	inline String ToSceneString(SceneName name) {
		switch (name) {
		case SceneName::Title:   return U"Title";
		case SceneName::Select:  return U"Select";
		case SceneName::Story:   return U"Story";
		case SceneName::InGame:  return U"InGame";
		case SceneName::Result:  return U"Result";
		default:                 return U"";
		}
	}
	class TitleScene : public App::Scene
	{
	private:
		Texture backgroundTexture;
		Texture logoTexture;
		Texture companyLogoTexture;

	public:
		// コンストラクタ
		TitleScene(const InitData& init)
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
		// 更新処理
		void update() override
		{
			const auto& inputs = Keyboard::GetAllInputs();

			// 何かキーが押されたかチェック
			for (const auto& input : inputs)
			{
				if (input.down())
				{
					changeScene(ToSceneString(SceneName::Select), 1.0);
					break;
				}
			}

			if(MouseL.down()||MouseR.down())
			{
				changeScene(ToSceneString(SceneName::Select), 1.0);
			}
		}

		// 描画処理
		void draw() const override
		{
			const double w = Scene::Width();
			const double h = Scene::Height();

			backgroundTexture.scaled(w / backgroundTexture.width()).draw(0, 0);
			logoTexture.scaled(w / logoTexture.width()*0.5).draw(0, h / 2.3);
			companyLogoTexture.scaled(w / companyLogoTexture.width() * 0.11).draw(w / 1.2, h/1.2);
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
	};
}
