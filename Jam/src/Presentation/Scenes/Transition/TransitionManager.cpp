#include "TransitionManager.h"
#include "RectSlide.h" 

namespace Jam::Presentation::Scenes
{
	TransitionManager& TransitionManager::Instance()
	{
		static TransitionManager instance;
		return instance;
	}

	// 必要なエフェクトを登録
	TransitionManager::TransitionManager()
	{
		// RectSlideを「画面サイズ」と「分割数30」で初期化して登録
		m_transitions[TransitionType::RectSlide] = std::make_unique<RectSlide>(Scene::Size(), 30);
	}

	TransitionManager::~TransitionManager() = default;


	void TransitionManager::refresh(TransitionType type)
	{
		if (auto* p = getTransition(type))
			p->refresh();
	}

	void TransitionManager::drawFadeOut(TransitionType type, double t)
	{
		if (!m_isFadingOut)
		{
			refresh(type);
			m_isFadingOut = true;
		}

		if (auto* p = getTransition(type))
			p->drawFadeOut(t);
	}

	void TransitionManager::drawFadeIn(TransitionType type, double t)
	{
		// 次回の遷移のためにフラグをリセット
		if (m_isFadingOut)
			m_isFadingOut = false;

		if (auto* p = getTransition(type))
			p->drawFadeIn(t);
	}

	ITransitionable* TransitionManager::getTransition(TransitionType type)
	{
		if (m_transitions.contains(type))
			return m_transitions[type].get();

		return m_transitions[TransitionType::RectSlide].get();
	}
}
