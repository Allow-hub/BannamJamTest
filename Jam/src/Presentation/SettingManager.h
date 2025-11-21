#pragma once
#include <Siv3D.hpp>
#include "../Infrastructure/IndependentObjectFactory.h"

namespace Jam::Presentation
{
	//設定画面管理クラス
	class SettingManager
	{
	private:
		SettingManager();
		Font font;
		Texture m_backgroundTexture;// 背景テクスチャ
		Texture m_backgroundMaskTexture;// 背景マスク?加工テクスチャ
		Texture m_creditTexture;// クレジットテクスチャ

		SceneManager<String>* m_sceneManager = nullptr;

		enum class Mode { Menu, Audio, Credit };
		Mode m_mode = Mode::Menu;

		struct Button
		{
			String label;
			Vec2 pos;
			int size;
			RectF rect;
			double underlineWidth = 0.0;
			double underlineTarget = 0.0;

			Button(const String& l, const Vec2& p, int s, double maxWidth);
			bool isHovered() const;
			void update();
			void draw(const Font& font);
		};

		Array<Button> buttons;

	public:
		static SettingManager& Instance();
		//シーン遷移に必要なSceneManagerのセット
		void setSceneManager(SceneManager<String>* manager);
		void changeScene(const String& sceneName);
		void init();
		void update();
		void draw();
	};
}
