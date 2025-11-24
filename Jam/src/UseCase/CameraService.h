#pragma once
#include "../Domain/Player/Player.h"
#include "../Presentation/CameraManager.h"
#include "CameraEvent.h"

namespace Jam::UseCase
{
	//カメラの更新
	class CameraService
	{
	private:
		Domain::Player::Player& m_player;
		Jam::Presentation::CameraManager& m_cameraManager;
		CameraEventQueue& m_cameraEventQueue;
		Vec2 m_offset = { 200, -150 };

		void processEvents();

	public:
		CameraService(Domain::Player::Player& player,
					  Jam::Presentation::CameraManager& camera,
					  CameraEventQueue& cameraEventQueue);

		void update(double deltaTime);
	};
}
