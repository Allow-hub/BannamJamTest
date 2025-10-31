#include "Player.h"
#include "../Physics/IPhysicsBody.h"
#include "Skill/ChokerSkill.h"
#include "../../Presentation/AudioService.h"
#include "../../Infrastructure/PhysicsFilterManager.h"
#include "../../Foundation/CoreManager.h"

namespace Jam::Domain::Player
{
	Player::Player(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Events::GameEventQueue& eventQueue)
		: m_body(std::move(body)), m_eventQueue(eventQueue)
	{
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::Player);
		m_body->setGravityScale(1.5);
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::Team1);
		m_body->setSleepEnabled(false);

		auto chokerSkill = std::make_shared<ChokerSkill>(eventQueue, m_body->getID(), m_stats, *this);
		chokerSkill->init(); // shared_from_this()を使用する初期化
		m_skills.push_back(chokerSkill);
		m_currentSkill = m_skills.front();
		auto& core = Jam::Foundation::CoreManager::Instance();
		m_fallLimitY = core.getStageData(core.stageInfo.stageName).fallLimitY;
	}

	void Player::update(double deltaTime)
	{
		updateState();
	
		if (m_isRespawning || !m_canControl)return;

		// --- 落下中に下向きの力を追加 ---
		auto velocity = m_body->getVelocity();

		if (velocity.y > 0) // yが正なら下方向
		{
			double fallAccelerationBase = 3000.0;
			double maxFallSpeed = 10000.0; // 最大落下速度

			// 現在の速度が上限を超えていないときだけ力を加える
			if (velocity.y < maxFallSpeed)
				m_body->applyForce({ 0, fallAccelerationBase });
			else
				m_body->setVelocity({ velocity.x, maxFallSpeed });
		}

		// 全スキルを更新（アクティブ/非アクティブ問わず、必要なものだけ）
		for (auto& skill : m_skills)
		{
			if (skill->needUpdate())
				skill->update(deltaTime);
		}

		if (getPosition().y >= m_fallLimitY)
		{
			respawn();
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
			Jam::Presentation::AudioService::get().playOneShot(Jam::Presentation::AudioService::Sound::SE_Jump, 0.5);
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

	Jam::Util::Task Player::respawn()
	{
		if (m_isRespawning)co_return;
		m_isRespawning = true;
		// CoreManager参照
		auto& core = Jam::Foundation::CoreManager::Instance();
		if (core.getDied())co_return;
		if (m_currentSkill)
			m_currentSkill->onDeactivate();
		const Vec2 respawnPos = core.getStageData(core.stageInfo.stageName).respawnPosition;
		m_eventQueue.push(Events::PlayerFallOutEvent{ 1.2, 0.4, 100 ,m_body->getPosition() });
		co_await Jam::Util::WaitSeconds(1.0);

		Jam::Presentation::FadeManager::instance().fadeOutAndIn();
		co_await Jam::Util::WaitSeconds(1.0);

		m_stats.hp -= 10;//リスポーン時のダメージ
		// --- リスポーン処理 ---
		m_body->setPos(respawnPos);
		m_body->setVelocity({ 0, 0 });
		m_isRespawning = false;
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
		//if (m_currentSkill)
		//	m_currentSkill->onDeactivate();
		// direction: 1 = ホイール上（次のスキル）、-1 = ホイール下（前のスキル）
		//auto it = std::find(m_skills.begin(), m_skills.end(), m_currentSkill);
		//if (it == m_skills.end())
		//{
		//	m_currentSkill = m_skills.front();
		//	return;
		//}

		//// 次のスキル or 前のスキルに移動
		//int index = static_cast<int>(std::distance(m_skills.begin(), it));
		//index += direction;

		//// 循環させる
		//if (index < 0) index = static_cast<int>(m_skills.size()) - 1;
		//else if (index >= static_cast<int>(m_skills.size())) index = 0;

		//m_currentSkill = m_skills[index];
	}

	void Player::takeDamage(const DamageInfo& info)
	{
		if (!m_isAlive) return;
		if (m_isInvincible)return;

		m_stats.hp -= info.amount;
		if (m_onDamaged) m_onDamaged();

		if (m_stats.hp <= 0)
		{
			m_stats.hp = 0;
			m_isAlive = false;
			onDeath(); // 死んだとき
		}
		else
		{
			onDamaged(info);//ダメージを受けた時の吹き飛ばし等
		}
	}

	void Player::setOnDamagedCallback(DamageCallback callback)
	{
		m_onDamaged = std::move(callback);
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

	void Player::controlCooldown(double cooldown)
	{
		m_canControl = false;
		controlCooldownProcess(cooldown);
	}

	Jam::Util::Task Player::controlCooldownProcess(double cooldown)
	{
		co_await Jam::Util::WaitSeconds(cooldown);
		m_canControl = true;
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

	void Player::onDamaged(const DamageInfo& info)
	{
		Jam::Presentation::AudioService::get().playOneShot(Jam::Presentation::AudioService::Sound::SE_Damage, 0.2);
		m_body->applyImpulse(info.direction * 1000);
	}

	void Player::onDeath()
	{
		auto& core = Jam::Foundation::CoreManager::Instance();
		core.setClear(false);
		m_eventQueue.push(Events::PlayerDeathEvent{ 1.2, 0.2, 10000 });
		Print << U"Player Died";
	}

	void Player::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		switch (other->getLayer())
		{
		case Jam::Domain::Physics::PhysicsLayer::Ground:
		{
			auto v = m_body->getVelocity();
			// 下向きまたは静止中に着地判定(上向きや横からの衝突は無視)
			if (v.y >= 0) {
				m_body->setVelocity({ v.x, 0.0 });
				m_isGrounded = true;
				m_jumpCount = 0;
			}
		}
		break;
		case Jam::Domain::Physics::PhysicsLayer::Wall:
		{
			auto v = m_body->getVelocity();
			m_body->setVelocity({ v.x, 0.0 });
		}
		break;
		case Jam::Domain::Physics::PhysicsLayer::Enemy:
			break;
		default:
			break;
		}
	}

	void Player::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}

	void Player::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
}
