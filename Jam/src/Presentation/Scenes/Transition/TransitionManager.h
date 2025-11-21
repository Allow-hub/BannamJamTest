#pragma once
#include <Siv3D.hpp>
#include <memory>
#include "ITransition.h"
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

		/**
		 * @brief 共有エフェクトの FADE OUT 描画
		 * @param t 進行度 (0.0 ～ 1.0)
		 */
		void drawFadeOut(double t) const
		{
			if (m_transition)
			{
				m_transition->drawFadeOut(t);
			}
		}

		/**
		 * @brief 共有エフェクトの FADE IN 描画
		 * @param t 進行度 (0.0 ～ 1.0)
		 */
		void drawFadeIn(double t) const
		{
			if (m_transition)
			{
				m_transition->drawFadeIn(t);
			}
		}

		/**
		 * @brief ランダムにするためにエフェクトの形状や色をリセット（再初期化）
		 * @param amount 初期化時の要素数
		 */
		void reset(int32 amount = 30)
		{
			if (m_transition)
			{
				m_transition->init(amount);
			}
		}

		/**
		 * @brief 管理するトランジションエフェクトを設定（変更）します。
		 * @param transition 新しいトランジションエフェクトのユニークポインタ
		 */
		void setTransition(std::unique_ptr<ITransition> transition)
		{
			m_transition = std::move(transition);
		}

	private:
		std::unique_ptr<ITransition> m_transition;

		TransitionManager()
			: m_transition(std::make_unique<RectSlide>(Scene::Size()))
		{
		}

		~TransitionManager() = default;

		TransitionManager(const TransitionManager&) = delete;
		TransitionManager& operator=(const TransitionManager&) = delete;
		TransitionManager(TransitionManager&&) = delete;
		TransitionManager& operator=(TransitionManager&&) = delete;
	};
}
