#pragma once
#include <Siv3D.hpp>
#include <memory>
#include <unordered_map> 
#include <functional> 
#include "ITransitionable.h"
#include "RectSlide.h" 

namespace Jam::Presentation::Scenes
{

	class TransitionManager
	{
	public:

		static TransitionManager& Instance()
		{
			static TransitionManager instance;
			return instance;
		}

		// --- publicな操作メソッド ---

		void changeTransition(TransitionType type)
		{
			if (m_factories.contains(type))
			{
				m_transition = m_factories[type]();
			}
		}

		//共有エフェクトの FADE OUT 描画
		// t 進行度 (0.0 ～ 1.0)

		void drawFadeOut(double t) const
		{
			if (m_transition)
			{
				m_transition->drawFadeOut(t);
			}
		}

		//共有エフェクトの FADE IN 描画
		// t 進行度 (0.0 ～ 1.0)
		void drawFadeIn(double t) const
		{
			if (m_transition)
			{
				m_transition->drawFadeIn(t);
			}
		}

		//ランダムにするためにエフェクトの形状や色をリセット（再初期化）
		//amount 初期化時の要素数
		void reset(int32 amount = 30)
		{
			if (m_transition)
			{
				m_transition->init(amount);
			}
		}

	private:
		std::unique_ptr<ITransitionable> m_transition;
		std::unordered_map<TransitionType, std::function<std::unique_ptr<ITransitionable>()>> m_factories;

		TransitionManager()
		{
			m_factories[TransitionType::RectSlide] = []() {
				return std::make_unique<RectSlide>(Scene::Size());
				};

			changeTransition(TransitionType::RectSlide);
		}

		~TransitionManager() = default;

		TransitionManager(const TransitionManager&) = delete;
		TransitionManager& operator=(const TransitionManager&) = delete;
		TransitionManager(TransitionManager&&) = delete;
		TransitionManager& operator=(TransitionManager&&) = delete;
	};
}
