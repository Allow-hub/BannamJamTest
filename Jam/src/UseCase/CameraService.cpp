#include "CameraService.h"
#include <Siv3D.hpp>

namespace Jam::UseCase
{
	CameraService::CameraService(Domain::Player::Player& player,
								 Jam::Presentation::CameraManager& camera,
								 CameraEventQueue& cameraEventQueue)
		: m_player(player)
		, m_cameraManager(camera)
		, m_cameraEventQueue(cameraEventQueue)
	{
	}

	void CameraService::update(double deltaTime)
	{
		// カメライベント処理
		processEvents();

		// 通常の追従
		if (m_cameraManager.getMode() == Jam::Presentation::CameraMode::FollowPlayer)
		{
			Vec2 playerPos = m_player.getPosition() + m_offset;
			m_cameraManager.setTarget(playerPos);
		}

		// デバッグ操作	
		if (KeyQ.pressed()) m_cameraManager.setZoom(0.01, 10.0);
		if (KeyE.pressed()) m_cameraManager.setZoom(1.2, 1.0);

		m_cameraManager.update(deltaTime);
	}

	void CameraService::processEvents()
	{
		while (!m_cameraEventQueue.empty())
		{
			auto event = m_cameraEventQueue.pop();

			std::visit([this](auto&& e) {
				using T = std::decay_t<decltype(e)>;

				if constexpr (std::is_same_v<T, CameraShakeEvent>)
				{
					m_cameraManager.shake(e.intensity, e.duration);
				}
				else if constexpr (std::is_same_v<T, CameraFocusEvent>)
				{
					m_cameraManager.focusOn(e.target, e.duration, e.zoom);
				}
				else if constexpr (std::is_same_v<T, CameraZoomEvent>)
				{
					m_cameraManager.setZoom(e.zoom, e.duration);
				}
				else if constexpr (std::is_same_v<T, CameraFollowPlayerEvent>)
				{
					m_cameraManager.resetToFollow();
				}
			}, event);
		}
	}
}
