#pragma once
#include <Siv3D.hpp>
#include <memory>
#include <unordered_map> 
#include <functional> 
#include "ITransitionable.h"

namespace Jam::Presentation::Scenes
{
	// シーン遷移（トランジション）などエフェクト全体を管理するクラス
	class TransitionManager
	{
	public:
		static TransitionManager& Instance();

		// 指定されたトランジションの乱数パターンを再生成する
		void refresh(TransitionType type);

		// 共有エフェクトの FADE OUT 描画
		// t : 進行度 (0.0 ～ 1.0)
		void drawFadeOut(TransitionType type, double t);

		// 共有エフェクトの FADE IN 描画
		// t : 進行度 (0.0 ～ 1.0)
		void drawFadeIn(TransitionType type, double t);

	private:
		TransitionManager();

		~TransitionManager();

		TransitionManager(const TransitionManager&) = delete;
		TransitionManager& operator=(const TransitionManager&) = delete;

		// Enum に対応するトランジションクラスを取得する
		ITransitionable* getTransition(TransitionType type);

		// トランジションの実体を管理する辞書
		std::unordered_map<TransitionType, std::unique_ptr<ITransitionable>> m_transitions;

		// フェードアウト中かどうかの状態フラグ（自動リフレッシュ制御用）
		bool m_isFadingOut = false;
	};
}
