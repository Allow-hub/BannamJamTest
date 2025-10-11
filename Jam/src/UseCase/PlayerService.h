#pragma once
#include "../Domain/Player/Player.h"
#include "../Domain/IInputService.h"

namespace Jam::UseCase
{
    // PlayerとInputServiceを結びつける、入力の反映
    class PlayerService
    {
	private:
        std::shared_ptr<Domain::Player> m_player;
		Domain::IInputService& m_input;

    public:
        PlayerService(const std::shared_ptr<Domain::Player>& player,Domain::IInputService& input)
            : m_player(player), m_input(input)
        {}

        void update(double deltaTime)
        {
			m_input.Update();
			Domain::InputState inputState = m_input.GetState();// ここでinputStateをPlayerに反映
			if (inputState.left)  m_player->moveLeft();
			if (inputState.right) m_player->moveRight();
			if (inputState.jump)  m_player->jump();
            m_player->update(deltaTime);
        }

        std::shared_ptr<Domain::Player> getPlayer() const { return m_player; }
    };
}