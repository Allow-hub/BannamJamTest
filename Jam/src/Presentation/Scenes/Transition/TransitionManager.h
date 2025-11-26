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

		void refresh(TransitionType type)
		{
			if (auto* p = getTransition(type))
			{
				p->refresh();
			}
		}

		//共有エフェクトの FADE OUT 描画
		// t 進行度 (0.0 ～ 1.0)
		void drawFadeOut(TransitionType type, double t)
		{
			if (!m_isFadingOut)
			{
				refresh(type);
				m_isFadingOut = true;
			}

			if (auto* p = getTransition(type))
			{
				p->drawFadeOut(t);
			}
		}

		//共有エフェクトの FADE IN 描画
		// t 進行度 (0.0 ～ 1.0)
		void drawFadeIn(TransitionType type, double t)
		{
			if (m_isFadingOut)
			{
				m_isFadingOut = false;
			}

			if (auto* p = getTransition(type))
			{
				p->drawFadeIn(t);
			}
		}

	private:
		RectSlide m_rectSlide;

		bool m_isFadingOut = false;

		TransitionManager()
			: m_rectSlide(Scene::Size(), 30) 
		{
		}

		~TransitionManager() = default;
		TransitionManager(const TransitionManager&) = delete;
		TransitionManager& operator=(const TransitionManager&) = delete;

		ITransitionable* getTransition(TransitionType type)
		{
			switch (type)
			{
			case TransitionType::RectSlide:
				return &m_rectSlide;

				// 将来エフェクトを追加する場合：
				// case TransitionType::CircleWipe:
				//    return &m_circleWipe;

			default:
				return nullptr;
			}
		}
	};
}
