#pragma once
#include "../Domain/Player/Player.h"
#include "../Domain/IInputService.h"
#include "../Presentation/PlayerManager.h"

namespace Jam::UseCase
{
	// PlayerとInputServiceを結びつける、入力の反映
	class PlayerService
	{
	private:
		std::shared_ptr<Domain::Player::Player> m_player;
		Domain::IInputService& m_input;
		Jam::Presentation::PlayerManager& m_manager;

	public:
		PlayerService(const std::shared_ptr<Domain::Player::Player>& player,
					  Domain::IInputService& input,
					  Jam::Presentation::PlayerManager& manager)
			: m_player(player), m_input(input), m_manager(manager)
		{
		}

		void update(double deltaTime)
		{
			m_input.Update();
			Domain::InputState inputState = m_input.GetState();

			bool isRunning = false;

			if (inputState.left)
			{
				m_player->moveLeft();
				m_manager.setFacingLeft(true);//左向きにテクスチャを反転
				isRunning = true;
			}
			if (inputState.right)
			{
				m_player->moveRight();
				m_manager.setFacingLeft(false);
				isRunning = true;
			}
			if (inputState.jump)
			{
				m_player->jump();
			}
			if (inputState.attack)
			{
				m_player->attack();
			}

			// どちらのキーも押されていないなら走行アニメをオフに
			m_manager.SetRunning(isRunning);

			m_player->update(deltaTime);
		}

		std::shared_ptr<Domain::Player::Player> getPlayer() const { return m_player; }

	};
}
