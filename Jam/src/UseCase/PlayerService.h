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

		double m_wheelAccumulator = 0.0; // 累積ホイール量
		const double wheelThreshold = 1.0; // 1回転で切り替え

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

			bool isRunning = false;

			if (inputState.left)
			{
				m_player->moveLeft();
				m_manager.setFacingLeft(true);
				isRunning = true;
			}
			if (inputState.right)
			{
				m_player->moveRight();
				m_manager.setFacingLeft(false);
				isRunning = true;
			}
			if (inputState.dash)
			{
				m_player->startDash();
			}
			else
			{
				m_player->endDash();
			}
			if (inputState.jump)
			{
				m_player->jump();
			}
			if (inputState.attack)
			{
				m_player->attack();
			}
			if (inputState.skillPush) // Skill ボタンが押されていたら
			{
				m_player->skillPush();
			}
			if (inputState.skillReleased) // Skill ボタンが離れてたら
			{
				m_player->skillReleased();
			}
			
			// 下ボタンの状態を更新
			m_player->setPressingDown(inputState.down);

			// ---------------------------------
			// マウスホイールでスキル切り替え
			double wheelDelta = inputState.skillChange; // 1フレームのホイール差分
			m_wheelAccumulator += wheelDelta;

			while (m_wheelAccumulator >= wheelThreshold)
			{
				m_player->changeSkill(1);
				m_wheelAccumulator -= wheelThreshold;
			}
			while (m_wheelAccumulator <= -wheelThreshold)
			{
				m_player->changeSkill(-1);
				m_wheelAccumulator += wheelThreshold;
			}
			// ---------------------------------

			m_manager.SetRunning(isRunning);

			m_player->update(deltaTime);
		}

		std::shared_ptr<Domain::Player::Player> getPlayer() const { return m_player; }
	};
}
