#pragma once
#include <Siv3D.hpp>

namespace Jam::Presentation
{
	class FadeManager
	{
	public:
		enum class FadeState
		{
			None,
			FadeOut,
			FadeIn
		};

	private:
		FadeState m_state = FadeState::None;
		double m_progress = 0.0;
		double m_duration = 1.0;
		ColorF m_fadeColor = ColorF{ 0, 0, 0 };
		std::function<void()> m_onFadeOutComplete = nullptr;
		std::function<void()> m_onFadeInComplete = nullptr;

		FadeManager() = default;

	public:
		static FadeManager& instance()
		{
			static FadeManager instance;
			return instance;
		}

		// コピー・ムーブ禁止
		FadeManager(const FadeManager&) = delete;
		FadeManager& operator=(const FadeManager&) = delete;

		/// @brief フェードアウトを開始
		/// @param duration フェード時間（秒）
		/// @param onComplete フェードアウト完了時のコールバック
		/// @param color フェードの色
		void startFadeOut(double duration = 1.0,
						 std::function<void()> onComplete = nullptr,
						 const ColorF& color = ColorF{ 0, 0, 0 })
		{
			if (m_state != FadeState::None) return;

			m_state = FadeState::FadeOut;
			m_progress = 0.0;
			m_duration = duration;
			m_fadeColor = color;
			m_onFadeOutComplete = onComplete;
		}

		/// @brief フェードインを開始
		/// @param duration フェード時間（秒）
		/// @param onComplete フェードイン完了時のコールバック
		void startFadeIn(double duration = 1.0,
						std::function<void()> onComplete = nullptr)
		{
			m_state = FadeState::FadeIn;
			m_progress = 0.0;
			m_duration = duration;
			m_onFadeInComplete = onComplete;
		}

		/// @brief フェードアウト→コールバック→フェードイン
		/// @param fadeOutDuration フェードアウト時間
		/// @param fadeInDuration フェードイン時間
		/// @param onMiddle フェードアウト完了時に実行する処理
		void fadeOutAndIn(double fadeOutDuration = 1.0,
						 double fadeInDuration = 1.0,
						 std::function<void()> onMiddle = nullptr)
		{
			startFadeOut(fadeOutDuration, [this, fadeInDuration, onMiddle]() {
				if (onMiddle) onMiddle();
				startFadeIn(fadeInDuration);
			});
		}

		void update(double deltaTime)
		{
			if (m_state == FadeState::None) return;

			m_progress += deltaTime / m_duration;

			if (m_progress >= 1.0)
			{
				m_progress = 1.0;

				if (m_state == FadeState::FadeOut)
				{
					if (m_onFadeOutComplete)
					{
						auto callback = m_onFadeOutComplete;
						m_onFadeOutComplete = nullptr;
						callback(); // コールバック実行
					}

					// フェードインが開始されていなければ終了
					if (m_state == FadeState::FadeOut)
					{
						m_state = FadeState::None;
					}
				}
				else if (m_state == FadeState::FadeIn)
				{
					if (m_onFadeInComplete)
					{
						auto callback = m_onFadeInComplete;
						m_onFadeInComplete = nullptr;
						callback();
					}
					m_state = FadeState::None;
				}
			}
		}

		void draw() const
		{
			if (m_state == FadeState::None) return;

			double alpha = 0.0;

			if (m_state == FadeState::FadeOut)
			{
				alpha = Math::Clamp(m_progress, 0.0, 1.0);
			}
			else if (m_state == FadeState::FadeIn)
			{
				alpha = Math::Clamp(1.0 - m_progress, 0.0, 1.0);
			}

			Scene::Rect().draw(ColorF{ m_fadeColor, alpha });
		}

		bool isFading() const
		{
			return m_state != FadeState::None;
		}

		FadeState getState() const
		{
			return m_state;
		}
	};
}
