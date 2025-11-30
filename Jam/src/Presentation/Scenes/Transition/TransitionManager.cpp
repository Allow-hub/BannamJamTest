#include "Presentation/Scenes/Transition/TransitionManager.h"
#include "Presentation/Scenes/Transition/RectSlide.h"
#include "Presentation/Scenes/Transition/IrisTransition.h"

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
		m_transitions[TransitionType::RectSlide] = std::make_unique<RectSlide>(Scene::Size());
		m_transitions[TransitionType::Iris]
			= std::make_unique<IrisTransition>(Scene::Size(), Palette::Black);
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
