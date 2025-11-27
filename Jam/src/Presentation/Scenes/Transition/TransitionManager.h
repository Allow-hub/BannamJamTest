#pragma once
#include <Siv3D.hpp>
#include <memory>
#include <unordered_map> 
#include <functional> 
#include "ITransitionable.h"
#include "RectSlide.h" 

namespace Jam::Presentation::Scenes
{
	// シーン遷移（トランジション）全体を管理するクラス
	class TransitionManager
	{
	public:
		static TransitionManager& Instance()
		{
			static TransitionManager instance;
			return instance;
		}

		// 指定されたトランジションの乱数パターンを再生成する
		void refresh(TransitionType type)
		{
			if (auto* p = getTransition(type))
				p->refresh();
		}

		// 共有エフェクトの FADE OUT 描画
		// t 進行度 (0.0 ～ 1.0)
		void drawFadeOut(TransitionType type, double t)
		{
			if (!m_isFadingOut)
			{
				refresh(type);
				m_isFadingOut = true;
			}

			if (auto* p = getTransition(type))
				p->drawFadeOut(t);
		}

		// 共有エフェクトの FADE IN 描画
		// t 進行度 (0.0 ～ 1.0)
		void drawFadeIn(TransitionType type, double t)
		{
			// 次回の遷移のためにフラグをリセット
			if (m_isFadingOut)
				m_isFadingOut = false;

			if (auto* p = getTransition(type))
				p->drawFadeIn(t);
		}

	private:
		// トランジションの実体を管理する辞書
		std::unordered_map<TransitionType, std::unique_ptr<ITransitionable>> m_transitions;

		bool m_isFadingOut = false;

		// トランジションクラスを登録
		TransitionManager()
		{
			m_transitions[TransitionType::RectSlide] = std::make_unique<RectSlide>(Scene::Size(), 30);
		}

		~TransitionManager() = default;
		TransitionManager(const TransitionManager&) = delete;
		TransitionManager& operator=(const TransitionManager&) = delete;

		// Enum に対応するトランジションクラスを取得する
		ITransitionable* getTransition(TransitionType type)
		{
			if (m_transitions.contains(type))
				return m_transitions[type].get();

			return m_transitions[TransitionType::RectSlide].get();
		}
	};
}
