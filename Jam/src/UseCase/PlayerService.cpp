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

		auto& core = Jam::Foundation::CoreManager::Instance();
		const bool isClearing = core.getClear();

		// クリア状態での処理
		if (isClearing)
		{
			// ゴールから1秒遅延してクリアアニメを開始
			if (m_clearAnimPending && !m_clearAnimationPlayed)
			{
				m_clearAnimDelayTimer += deltaTime;
				if (m_clearAnimDelayTimer >= m_clearAnimDelay)
				{
					m_manager.setAnim(U"clearAnimationPlayed", true);
					m_clearAnimationPlayed = true;
					m_clearAnimPending = false;
				}
			}
			// 入力は無効化されたままなので操作系はスキップ
			// 代わりに現在状態からアニメフラグのみ更新
			const bool isJumping = (!m_player->getGrounded());
			const bool isChokerThrow = m_player->getIsChokering();

			m_manager.setAnim(U"isWalking", false);
			m_manager.setAnim(U"isRunning", false);
			m_manager.setAnim(U"isJumping", isJumping);
			m_manager.setAnim(U"isChokerThrow", isChokerThrow);

			// 物理処理は継続
			m_player->update(deltaTime);
			return;
		}

		if (m_player->getIsRespawning()) return;
		if (!m_player->getCanControl()) return;

		if (inputState.settting)
		{
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
		{
			m_manager.setAnim(U"isDamage", false);
		}

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

	void PlayerService::onGoalReached(const Vec2& goalPos)
	{
		// 入力無効化
		m_player->setCanControl(false);

		// 停止とワープ
		const Vec2 offset{ 0.0, -50.0 };
		if (auto body = m_player->getPhysicsBody())
		{
			body->setVelocity({ 0.0, 0.0 });
			body->setPos(goalPos + offset);// ゴールより少し上にワープさせる
		}
		// クリアアニメーション再生待ち状態に設定
		m_clearAnimPending = true;
		m_clearAnimDelayTimer = 0.0;
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
