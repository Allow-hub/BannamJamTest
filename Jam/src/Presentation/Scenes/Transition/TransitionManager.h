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
		void refresh(TransitionType type)
		{
			getTransition(type)->refresh();
		}

		//共有エフェクトの FADE OUT 描画
		// t 進行度 (0.0 ～ 1.0)

		void drawFadeOut(TransitionType type, double t)
		{
			// 指定されたタイプのエフェクトを取得（なければ生成）して描画
			getTransition(type)->drawFadeOut(t);
		}

		//共有エフェクトの FADE IN 描画
		// t 進行度 (0.0 ～ 1.0)
		void drawFadeIn(TransitionType type, double t)
		{
			getTransition(type)->drawFadeIn(t);
		}

	private:
		std::unordered_map<TransitionType, std::unique_ptr<ITransitionable>> m_instances;

		TransitionManager() = default;
		~TransitionManager() = default;

		TransitionManager(const TransitionManager&) = delete;
		TransitionManager& operator=(const TransitionManager&) = delete;

		ITransitionable* getTransition(TransitionType type)
		{
			// まだ辞書にない場合、新しく作って登録する
			if (!m_instances.contains(type))
			{
				m_instances[type] = createInstance(type);
			}
			// 辞書にある実体のポインタを返す
			return m_instances[type].get();
		}


		//Enumに応じた具体的なクラスの生成を行う
		std::unique_ptr<ITransitionable> createInstance(TransitionType type)
		{
			switch (type)
			{
			case TransitionType::RectSlide:
				return std::make_unique<RectSlide>(Scene::Size());

			default:
				return std::make_unique<RectSlide>(Scene::Size());
			}
		}
	};
}
