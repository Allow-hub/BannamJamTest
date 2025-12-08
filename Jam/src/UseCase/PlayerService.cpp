#include "UseCase/PlayerService.h"
#include "Foundation/CoreManager.h"

namespace Jam::UseCase
{
	PlayerService::PlayerService(
		const std::shared_ptr<Domain::Player::Player>& player,
		Domain::IInputService& input,
		Jam::Presentation::PlayerManager& manager)
		: m_player(player), m_input(input), m_manager(manager)
	{
		AttackProcessor::getInstance().registerDamageable(
			m_player->getPhysicsBody()->getID(),
			m_player
		);
		m_player->addOnDamagedCallback([this]() { this->onPlayerDamaged(); });
	}

	void PlayerService::update(double deltaTime)
	{
		m_input.Update();
		Domain::InputState inputState = m_input.GetState();

		if (m_player->getIsRespawning()) return;// リスポーン中は操作不可
		if (!m_player->getCanControl()) return;// 操作不可状態なら何もしない

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

		if (!m_player->getGrounded()) isJumping = true;

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
		{
			m_player->endDash();
		}

		if (inputState.jump)
		{
			m_player->jump();
			isJumping = true;
		}

		if (damageTimer.reachedZero())
			m_manager.setAnim(U"isDamage", false);

		if (inputState.chokerPush) m_player->chokerPush();
		if (inputState.chokerReleased) m_player->chokerReleased();

		isChokerThrow = m_player->getIsChokering();

		m_player->setPressingDown(inputState.down);

		// 🔹 アニメーション状態を更新
		m_manager.setAnim(U"isWalking", isWalking);
		m_manager.setAnim(U"isRunning", isRunning);
		m_manager.setAnim(U"isJumping", isJumping);
		m_manager.setAnim(U"isChokerThrow", isChokerThrow);

		m_player->update(deltaTime);
	}
	void PlayerService::onPlayerDamaged()
	{
		// ダメージを受けたときの処理
		m_manager.setAnim(U"isDamage", true);
		damageTimer.restart();
	}

	std::shared_ptr<Domain::Player::Player> PlayerService::getPlayer() const
	{
		return m_player;
	}
}
