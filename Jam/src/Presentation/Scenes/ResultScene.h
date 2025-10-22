#pragma once
#include <Siv3D.hpp> // Siv3D v0.6.14
#include "TitleScene.h"
#include "../../Foundation/CoreManager.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;
	class ResultScene : public App::Scene
	{
	private:
		bool m_isClear;//クリアしたかどうか
		int m_flagmentAmount;//現在のステージでゲットした記憶のかけら
		int m_maxFlagment;//最大の記憶のかけら
		unsigned int m_time;

	public:
		// コンストラクタ
		ResultScene(const InitData& init)
			: IScene{ init }
		{
			auto& core = Jam::Foundation::CoreManager::Instance();
			m_isClear = core.getClear();
			m_flagmentAmount = core.getFlagment();
			m_maxFlagment = core.getMaxFlagment();
			m_time = static_cast<unsigned int>(core.getTimer());
		}

		// 更新処理
		void update() override
		{
		}

		// 描画処理
		void draw() const override
		{
		}
	};
}
