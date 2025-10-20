#include "Player.h"
#include "../Physics/IPhysicsBody.h"
#include "Skill/BombSkill.h"
#include "Skill/ChokerSkill.h"

namespace Jam::Domain::Player
{
	Player::Player(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Events::GameEventQueue& eventQueue)
		: m_body(std::move(body)), m_eventQueue(eventQueue)
	{
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::Player);
		m_body->setGravityScale(1.5);
		auto chokerSkill = std::make_shared<ChokerSkill>(eventQueue, m_body->getID());
		chokerSkill->init(); // shared_from_this()を使用する初期化
		m_skills.push_back(chokerSkill);
		m_skills.push_back(std::make_shared<BombSkill>(eventQueue));
		m_currentSkill = m_skills.front();
	}

	void Player::update(double deltaTime)
	{
		updateState();
		// 全スキルを更新（アクティブ/非アクティブ問わず、必要なものだけ）
		for (auto& skill : m_skills)
		{
			if (skill->needUpdate())
				skill->update(deltaTime);
		}
	}

	void Player::draw() const
	{
		// 全スキルを描画
		for (const auto& skill : m_skills)
		{
			if (skill->needUpdate())
				skill->draw();
		}
	}
	void Player::moveLeft()
	{
		double speedMultiplier = getHookedSpeedMultiplier();

		m_body->applyForce({ -m_stats.moveSpeed * speedMultiplier, 0 });
		m_facingRight = false;
	}

	void Player::moveRight()
	{
		double speedMultiplier = getHookedSpeedMultiplier();

		m_body->applyForce({ m_stats.moveSpeed * speedMultiplier, 0 });
		m_facingRight = true;
	}

	void Player::startDash()
	{
		if (m_isDashing)return;
		m_stats.moveSpeed *= dashMagnification;
		m_isDashing = true;
	}

	void Player::endDash()
	{
		if (!m_isDashing)return;
		m_stats.moveSpeed /= dashMagnification;
		m_isDashing = false;
	}

	void Player::jump()
	{
		if (m_isGrounded || m_jumpCount < maxJumpCount)
		{
			double jumpPower = m_stats.jumpPower;

			// 2回目のジャンプならジャンプ力を強化
			if (m_jumpCount == 1)
			{
				jumpPower *= 1.5;
			}

			m_body->setVelocity({ m_body->getVelocity().x, 0.0 });
			m_body->applyImpulse({ 0, -m_stats.jumpPower });
			m_jumpCount++;
			m_isGrounded = false;
		}
	}


	void Player::attack()
	{

	}

	//後々スキルはコンストラクタで使える武器をステージごとに選べるように
	void Player::skillPush()
	{
		if (m_currentSkill)
			m_currentSkill->use(getPosition(), m_facingRight);
	}
	void Player::skillReleased()
	{
		if (m_currentSkill)
			m_currentSkill->useReleased(getPosition(), m_facingRight);
	}
	void Player::changeSkill(int direction)
	{
		if (m_skills.empty()) return;
		if (m_currentSkill)
			m_currentSkill->onDeactivate();
		// direction: 1 = ホイール上（次のスキル）、-1 = ホイール下（前のスキル）
		auto it = std::find(m_skills.begin(), m_skills.end(), m_currentSkill);
		if (it == m_skills.end())
		{
			m_currentSkill = m_skills.front();
			return;
		}

		// 次のスキル or 前のスキルに移動
		int index = static_cast<int>(std::distance(m_skills.begin(), it));
		index += direction;

		// 循環させる
		if (index < 0) index = static_cast<int>(m_skills.size()) - 1;
		else if (index >= static_cast<int>(m_skills.size())) index = 0;

		m_currentSkill = m_skills[index];
	}


	s3d::Vec2 Player::getPosition() const
	{
		 return m_body->getPosition();
	}

	bool Player::isFacingRight() const
	{
		return m_facingRight;
	}

	void Player::updateState()
	{
	}

	double Player::getHookedSpeedMultiplier() const
	{
		// ChokerSkillを探す
		for (const auto& skill : m_skills)
		{
			if (skill->getType() == PlayerSkillType::Choker)
			{
				auto chokerSkill = std::dynamic_pointer_cast<ChokerSkill>(skill);
				if (chokerSkill && chokerSkill->isFlying())
				{
					return chokerSkill->getHookedMoveSpeedMultiplier();
				}
			}
		}
		return 1.0;  // フック中でなければ通常速度
	}
	void Player::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		switch (other->getLayer())
		{
		case Jam::Domain::Physics::PhysicsLayer::Ground:
			auto v = m_body->getVelocity();
			m_body->setVelocity({ v.x, 0.0 });
			m_isGrounded = true;
			m_jumpCount = 0;
			break;
		case Jam::Domain::Physics::PhysicsLayer::Enemy:
			break;
		default:
			//Print(U"Not match tag");
			break;
		}
	}

	void Player::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void Player::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
}
