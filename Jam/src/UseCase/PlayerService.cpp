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
			m_clearAnimDelayTimer += deltaTime;
			switch (m_clearAnimFase)
			{
			case 0:
				// ジャンプ
				if (m_clearAnimDelayTimer >= m_clearJumpDelay)
				{
					auto body = m_player->getPhysicsBody();
					body->setVelocity({ 0.0, 0.0 });
					body->applyImpulse({ 0.0, -800.0 });
					m_manager.setAnim(U"isJumping", true);
					m_clearAnimFase = 1;
				}
				break;
			case 1:
				// クリアアニメ再生
				if (m_clearAnimDelayTimer >= m_clearAnimDelay)
				{
					m_manager.setAnim(U"clearAnimationPlayed", true);
					m_clearAnimFase = 2;
				}
				break;
			case 2:
				// 再生終了
				break;
			default:
				break;
			}
			// 物理処理とアニメーションのみ更新
			m_player->update(deltaTime);

			// クリア演出1への以降直後は	ジャンプ状態を維持
			const bool isJumping =((m_clearAnimFase == 1) && (m_clearAnimDelayTimer <= m_clearJumpAnimDelay))|| (!m_player->getGrounded());
			m_manager.setAnim(U"isWalking", false);
			m_manager.setAnim(U"isRunning", false);
			m_manager.setAnim(U"isJumping", isJumping);
			m_manager.setAnim(U"isChokerThrow", false);
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

	// ゴールからの通知を受け取ったときの即時処理
	void PlayerService::onGoalReached(const Vec2& goalPos)
	{
		// 入力無効化
		m_player->setCanControl(false);
		// チョーカー解除
		m_player->chokerReleased();
		m_manager.setAnim(U"isChokerThrow", false);
		// 停止とワープ
		const Vec2 offset{ 0.0, -20.0 };
		if (auto body = m_player->getPhysicsBody())
		{
			body->setVelocity({ 0.0, 0.0 });
			body->setPos(goalPos + offset);// ゴールより少し上にワープさせる
		}
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
