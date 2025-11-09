#pragma once
#include "../Domain/Player/Player.h"
#include "../Domain/IInputService.h"
#include "../Presentation/PlayerManager.h"
#include "AttackProcessor.h"
#include <Siv3D.hpp>

namespace Jam::UseCase
{
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
			Jam::UseCase::AttackProcessor::getInstance().registerDamageable(m_player->getPhysicsBody()->getID(), m_player);
		}

		void update(double deltaTime)
		{
			m_input.Update();
			Domain::InputState inputState = m_input.GetState();
			if (m_player->getIsRespawning())return;
			if (!m_player->getCanControl())return;
			if (inputState.settting)
			{
				auto& core = Jam::Foundation::CoreManager::Instance();
				core.setPause(!core.getPause());
				return;
			}

			bool isRunning = false;
			bool isWalking = false;
			bool isJumping = false;
			bool isChokerThrow = false;

			if (!m_player->getGrounded())
				isJumping = true;

			if (inputState.left)
			{
				m_player->moveLeft(deltaTime);
				m_manager.setFacingLeft(true);
				isWalking = true;
			}
			if (inputState.right)
			{
				m_player->moveRight(deltaTime);
				m_manager.setFacingLeft(false);
				isWalking = true;
			}
			if (inputState.dash)
			{
				m_player->startDash();
				isRunning = true;
			}
			else
				m_player->endDash();
			if (inputState.jump){
				m_player->jump();
				isJumping = true;
			}
			if (inputState.chokerPush)
			{
				m_player->chokerPush();
			}
			if (inputState.chokerReleased)
			{
				m_player->chokerReleased();
			}

			isChokerThrow = m_player->getIsChokering() ? true : false;

			m_player->setPressingDown(inputState.down);


			// 🔹 アニメーション状態を更新
			m_manager.setAnim(U"isWalking", isWalking);
			m_manager.setAnim(U"isRunning", isRunning);
			m_manager.setAnim(U"isJumping", isJumping);
			m_manager.setAnim(U"isChokerThrow", isChokerThrow);

			m_player->update(deltaTime);
		}

		std::shared_ptr<Domain::Player::Player> getPlayer() const { return m_player; }
	};
}
