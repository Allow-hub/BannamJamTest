#include "Player.h"
#include "../Physics/IPhysicsBody.h"
#include "Choker/ChokerSkill.h"
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

		// チョーカーを初期化
		m_choker = std::make_shared<ChokerSkill>(eventQueue, m_body->getID(), m_stats, *this);
		m_choker->init();

		auto& core = Jam::Foundation::CoreManager::Instance();
		m_fallLimitY = core.getStageData(core.stageInfo.stageName).fallLimitY;
	}

	void Player::update(double deltaTime)
	{
		if (m_isRespawning || !m_canControl) return;

		// 落下加速
		auto velocity = m_body->getVelocity();
		double fallAcceleration = 1000.0;
		double maxFallSpeed = 3000.0;

		velocity.y += fallAcceleration * deltaTime;
		if (velocity.y > maxFallSpeed) velocity.y = maxFallSpeed;

		m_body->setVelocity({ velocity.x, velocity.y });

		// チョーカー更新
		if (m_choker && m_choker->needUpdate())
		{
			m_isChokering = true;
			m_choker->update(deltaTime);
		}
		else
		{
			m_isChokering = false;
		}

		// 落下判定
		if (getPosition().y >= m_fallLimitY)
		{
			respawn();
		}
	}

	void Player::draw() const
	{
		if (m_choker && m_choker->needUpdate())
		{
			m_choker->draw();
		}
	}

	void Player::moveLeft(double dt)
	{
		if (m_choker && m_choker->m_state->isHookedGround() && m_isDashing)
			m_body->applyForce({ -m_stats.moveSpeed * 6 * dt, 0 });
		else
			m_body->applyForce({ -m_stats.moveSpeed * dt, 0 });
		m_facingRight = false;
	}

	void Player::moveRight(double dt)
	{
		if (m_choker && m_choker->m_state->isHookedGround() && m_isDashing)
			m_body->applyForce({ m_stats.moveSpeed * 6 * dt, 0 });
		else
			m_body->applyForce({ m_stats.moveSpeed * dt, 0 });
		m_facingRight = true;
	}

	void Player::startDash()
	{
		if (m_isDashing) return;
		m_isDashing = true;
	}

	void Player::endDash()
	{
		if (!m_isDashing) return;
		m_isDashing = false;
	}

	void Player::jump()
	{
		if (m_isGrounded || m_jumpCount < maxJumpCount)
		{
			Jam::Presentation::AudioService::get().playOneShot(Jam::Presentation::AudioService::Sound::SE_Jump, 1.0);
			double jumpPower = m_stats.jumpPower;

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
		if (m_isRespawning) co_return;
		m_isRespawning = true;

		auto& core = Jam::Foundation::CoreManager::Instance();
		if (core.getDied()) co_return;

		if (m_choker)
			m_choker->onDeactivate();

		const Vec2 respawnPos = core.getStageData(core.stageInfo.stageName).respawnPosition;

		Jam::Presentation::AudioService::get().playOneShot(
			Jam::Presentation::AudioService::Sound::SE_FallDamage,
			0.6
		);

		m_eventQueue.push(Events::PlayerFallOutEvent{ 1.2, 0.4, 100, m_body->getPosition() });
		co_await Jam::Util::WaitSeconds(1.0);

		Jam::Presentation::FadeManager::instance().fadeOutAndIn();
		co_await Jam::Util::WaitSeconds(1.0);

		m_stats.hp -= 10;
		m_body->setPos(respawnPos);
		m_body->setVelocity({ 0, 0 });
		m_isRespawning = false;
	}

	void Player::chokerPush()
	{
		if (m_choker)
			m_choker->use(getPosition(), m_facingRight);
	}

	void Player::chokerReleased()
	{
		if (m_choker)
			m_choker->useReleased(getPosition(), m_facingRight);
	}

	void Player::takeDamage(const DamageInfo& info)
	{
		if (!m_isAlive) return;
		if (m_isInvincible) return;

		m_stats.hp -= info.amount;
		for (auto& cb : m_onDamagedCallbacks)
			cb();

		if (m_stats.hp <= 0)
		{
			m_stats.hp = 0;
			m_isAlive = false;
			onDeath();
		}
		else
		{
			onDamaged(info);
		}
	}
	void Player::addOnDamagedCallback(DamageCallback callback)
	{
		m_onDamagedCallbacks.push_back(std::move(callback));
	}

	Vec2 Player::getPosition() const { return m_body->getPosition(); }

	bool Player::isFacingRight() const { return m_facingRight; }

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
	}

	void Player::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		switch (other->getLayer())
		{
		case Jam::Domain::Physics::PhysicsLayer::Ground:
		{
			auto v = m_body->getVelocity();
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
