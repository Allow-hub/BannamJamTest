#pragma once
#include <variant>
#include <queue>

namespace Jam::UseCase
{
	//強さ,継続時間
	struct CameraShakeEvent
	{
		double intensity;
		double duration;
	};

	//ターゲット,時間,ズーム倍率
	struct CameraFocusEvent
	{
		Vec2 target;
		double duration;
		double zoom = 1.0;
	};

	//ズーム倍率,時間
	struct CameraZoomEvent
	{
		double zoom;
		double duration = 0.5;
	};

	struct CameraFollowPlayerEvent
	{
		// プレイヤー追従に戻す
	};

	//ターゲット,フォーカス継続時間,目標ズーム倍率,ズーム補間時間
	struct CameraFocusZoomTransitionEvent
	{
		Vec2 target;
		double focusDuration;
		double zoom;
		double zoomDuration;
	};

	using CameraEvent = std::variant<
		CameraShakeEvent,
		CameraFocusEvent,
		CameraZoomEvent,
		CameraFollowPlayerEvent,
		CameraFocusZoomTransitionEvent
	>;

	// カメラServiceに渡すQueue
	class CameraEventQueue
	{
	private:
		std::queue<CameraEvent> m_events;

	public:
		void push(const CameraEvent& event)
		{
			m_events.push(event);
		}

		bool empty() const
		{
			return m_events.empty();
		}

		CameraEvent pop()
		{
			auto event = m_events.front();
			m_events.pop();
			return event;
		}

		void clear()
		{
			while (!m_events.empty()) m_events.pop();
		}
	};
}
