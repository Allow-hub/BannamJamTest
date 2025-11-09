#include "ChokerSkill.h"
#include "../../../Infrastructure/Siv3DCursorUtil.h"
#include "../../../Infrastructure/FactoryServiceLocator.h"
#include "../../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../../Presentation/AudioService.h"
#include "../../../Infrastructure/PhysicsFilterManager.h"
#include "../Player.h"

#include <Siv3D.hpp>

using namespace Jam::Domain::Player;
using namespace Jam::Domain::Physics;

namespace Jam::Domain::Player
{
	ChokerSkill::ChokerSkill(Jam::Domain::Events::GameEventQueue& eventQueue,
							 PhysicsBodyID ownerId,
							 Jam::Domain::Player::PlayerStats& stats,
							 Player& player)
		: m_eventQueue(eventQueue)
		, m_ownerId(ownerId)
		, m_playerStats(stats)
		, m_player(player)
	{
		m_body = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
			.getPhysicsFactory()
			->createBody(
				Vec2{ -10000, -10000 },
				SizeF{ 20, 20 },
				s3d::P2BodyType::Dynamic,
				Jam::Domain::Physics::PhysicsMaterial{ 0.0, 0.0, 0.1 },
				Jam::Domain::Physics::PhysicsShape::Circle
			);

		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::PlayerWeapon);
		m_body->setGravityScale(0);
		m_body->setBullet(true);
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::Weapon);
	}

	void ChokerSkill::init()
	{
		m_body->setCollisionListener(shared_from_this());
	}

	void ChokerSkill::releaseJoint()
	{
		if (m_joint.has_value())
		{
			m_joint->release();
			m_joint.reset();
		}
		m_isJointCreated = false;
	}

	void ChokerSkill::resetHook()
	{
		m_body->setVelocity({ 0, 0 });
		m_body->setAngularVelocity(0);
		m_body->setBodyType(PhysicsType::Dynamic);
		m_body->setGravityScale(0);
	}

	void ChokerSkill::use(const Vec2 position, bool facingRight)
	{
		if (m_cooldownTimer > 0.0 || m_isActive || m_joint.has_value())
			return;

		Jam::Presentation::AudioService::get().playOneShot(Jam::Presentation::AudioService::Sound::SE_Choker, 0.3);
		m_flyTimer = 0.0;
		m_isInEnemyHitFreeze = false;
		m_enemyHitFreezeTimer = 0.0;
		m_isEnemySequenceActive = false;
		m_targetEnemy.reset();
		releaseJoint();
		resetHook();

		Vec2 target = Jam::Infrastructure::CursorUtil::instance().getCursorPosF();
		bool isRight = (target.x >= position.x);
		Vec2 offset = isRight ? createOffset : Vec2{ -createOffset.x, createOffset.y };
		Vec2 hookStartPos = position + offset;

		m_body->setPos(hookStartPos);

		Vec2 diff = target - hookStartPos;
		if (diff.isZero()) diff = Vec2{ 1, 0 };
		Vec2 dir = diff.normalized();

		const double speed = 5000.0;
		m_body->setVelocity(dir * speed);

		m_isActive = true;
		m_isFlying = true;
	}

	void ChokerSkill::useReleased(const Vec2 position, bool facingRight)
	{
		if (m_isEnemySequenceActive)
		{
			return;
		}

		if (!m_isActive && !m_joint.has_value() && !m_isFlying)
			return;

		releaseJoint();
		resetHook();
		m_body->setPos(Vec2{ -10000, -10000 });

		m_isFlying = false;
		m_isActive = false;

		m_cooldownTimer = m_cooldownTime;

		auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
			.getPhysicsFactory()
			->getBody(m_ownerId);

		Vec2 impulseDir = m_lastDir;
		if (!impulseDir.hasNaN() || impulseDir.isZero())
			impulseDir = facingRight ? Vec2{ 0, 0 } : Vec2{ 0, 0 };

		playerBody->applyImpulse(impulseDir * m_releaseImpulse);
	}

	void ChokerSkill::finishEnemySequence()
	{
		releaseJoint();
		resetHook();
		m_body->setPos(Vec2{ -10000, -10000 });

		m_isFlying = false;
		m_isActive = false;
		m_isEnemySequenceActive = false;
		m_isHooked = false;
		m_hookState = HookState::None;
		m_targetEnemy.reset();
		m_cooldownTimer = m_cooldownTime;
		delayReset();
	}

	Jam::Util::Task ChokerSkill::delayReset()
	{
		co_await Jam::Util::WaitSeconds(0.5);
		m_player.setIsInvincible(false);
	}

	void ChokerSkill::update(double deltaTime)
	{
		if (m_cooldownTimer > 0.0)
		{
			m_cooldownTimer = std::max(0.0, m_cooldownTimer - deltaTime);
		}

		if (m_isInEnemyHitFreeze)
		{
			m_enemyHitFreezeTimer -= deltaTime;
			if (m_enemyHitFreezeTimer <= 0.0)
			{
				m_isInEnemyHitFreeze = false;
				createEnemyJoint();
			}
			return;
		}

		if (m_isFlying)
		{
			m_flyTimer += deltaTime;
			if (m_flyTimer > m_maxFlyTime)
				useReleased({ -10000, -10000 }, true);
		}

		if (m_joint.has_value() && m_isJointCreated)
		{
			switch (m_hookState)
			{
			case HookState::HookedGround:
			{
				if (m_ground)
				{
					Vec2 groundPos = m_ground->getPosition();
					m_body->setPos(groundPos + m_groundAnchorOffset);
				}
				double currentMax = m_joint->getMaxLength();
				double newMax = currentMax * 0.992;
				double minLimit = 50.0;

				m_joint->setMaxLength(std::max(newMax, minLimit));
				if (newMax <= minLimit) m_isHooked = true;
				break;
			}
			case HookState::HookedEnemy:
			{
				if (m_targetEnemy)
				{
					auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
						.getPhysicsFactory()->getBody(m_ownerId);

					if (!playerBody)
					{
						finishEnemySequence();
						break;
					}

					double currentMax = m_joint->getMaxLength();
					double newMax = currentMax * m_enemyJointShrinkSpeed;
					double minLength = 30.0;
					m_joint->setMaxLength(std::max(newMax, minLength));

					if (newMax <= minLength + 50.0)
					{
						m_player.setIsInvincible(true);
					}

					if (newMax <= minLength + 1.0)
					{
						auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
							.getPhysicsFactory()->getBody(m_ownerId);

						if (!playerBody)
						{
							finishEnemySequence();
							return;
						}

						Vec2 playerPos = playerBody->getPosition();
						Vec2 enemyPos = m_targetEnemy->getPosition();
						double distance = (enemyPos - playerPos).length();

						const double reachThreshold = 160;
						if (distance > reachThreshold)
						{
							finishEnemySequence();
							return;
						}

						const double launchPower = 2000.0;
						m_player.controlCooldown(0.5);
						m_eventQueue.push(Events::PlayerAttackedEvent{ 1.2, 0.2, 25.2 });
						playerBody->applyImpulse(m_lastDir * launchPower);

						m_eventQueue.push(Events::EnemyDamagedEvent{
							playerBody->getID(),
							m_targetEnemy->getID(),
							DamageInfo {
								m_playerStats.power,
								m_body->getPosition(),
								m_enemyImpluseDir,
								true,
								false
							}
						});
						finishEnemySequence();
					}
				}
				else
				{
					finishEnemySequence();
				}
				break;
			}
			default: break;
			}
		}
	}

	void ChokerSkill::draw() const
	{
		if (!m_isActive) return;

		if (m_joint.has_value())
			m_joint->draw(Palette::Violet);

		m_body->drawFrame(3.0, Palette::Violet);
	}

	bool ChokerSkill::needUpdate() const
	{
		return m_isActive || m_cooldownTimer > 0.0 || m_isInEnemyHitFreeze;
	}

	void ChokerSkill::onCollisionEnter(std::shared_ptr<IPhysicsBody> other)
	{
		switch (other->getLayer())
		{
		case PhysicsLayer::Ground:
			m_ground = other;
			hitGround();
			break;
		case PhysicsLayer::Enemy:
			hitEnemy(other);
			break;
		case PhysicsLayer::Wall:
			m_body->setVelocity({ 0, 0 });
			break;
		}
	}

	void ChokerSkill::onCollisionStay(std::shared_ptr<IPhysicsBody> other) {}
	void ChokerSkill::onCollisionExit(std::shared_ptr<IPhysicsBody> other) {}

	void ChokerSkill::hitGround()
	{
		if (m_isFlying && !m_isJointCreated)
		{
			m_hookState = HookState::HookedGround;
			m_eventQueue.push(Events::PlayerChokerSkillEvent{ 0.8, 0.7 });
			m_body->setVelocity(Vec2::Zero());
			m_body->setAngularVelocity(0);
			m_body->setBodyType(PhysicsType::Static);
			m_isFlying = false;

			auto& world = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()->getWorld();

			Vec2 hookPos = m_body->getPosition();
			auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()->getBody(m_ownerId);

			if (!playerBody)
				return;

			Vec2 playerPos = playerBody->getPosition();
			double dist = (hookPos - playerPos).length();
			dist = std::clamp(dist, 1.0, 9999.0);

			releaseJoint();

			m_joint = m_body->createDistanceJoint(world, playerBody, hookPos, playerPos, dist);

			if (m_joint.has_value())
			{
				m_joint->setLinearStiffness(10.0, 1.0);
				m_joint->setMinLength(0.0);
				m_joint->setMaxLength(dist);

				Vec2 diff = hookPos - playerPos;
				if (diff.isZero()) diff = Vec2{ 0, -1 };
				m_lastDir = diff.normalized().rotated(-8_deg);

				if (m_ground)
				{
					Vec2 groundPos = m_ground->getPosition();
					m_groundAnchorOffset = hookPos - groundPos;
				}
				m_isJointCreated = true;
			}
			else
			{
				Print << U"⚠️ ジョイント作成失敗";
			}
		}
	}

	void ChokerSkill::hitEnemy(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> enemy)
	{
		if (!enemy || !m_isFlying || m_isJointCreated) return;

		m_hookState = HookState::HookedEnemy;
		m_targetEnemy = enemy;
		m_isEnemySequenceActive = true;
		m_isInEnemyHitFreeze = true;
		m_enemyHitFreezeTimer = m_enemyHitFreezeDuration;
		m_isFlying = false;

		auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
			.getPhysicsFactory()->getBody(m_ownerId);

		if (playerBody)
		{
			Vec2 hookPos = m_body->getPosition();
			Vec2 playerPos = playerBody->getPosition();
			Vec2 diff = hookPos - playerPos;

			if (diff.isZero())
				diff = Vec2{ 0, -1 };

			m_enemyImpluseDir = diff.normalized().rotated(-2_deg);
		}

		m_body->setVelocity({ 0, 0 });
		enemy->setVelocity(Vec2::Zero());
		m_body->setAngularVelocity(0);

		m_eventQueue.push(Events::PlayerChokerSkillEvent{ 0.9, 0.5 });
	}

	void ChokerSkill::createEnemyJoint()
	{
		if (!m_targetEnemy)
		{
			Print << U"⚠️ ターゲット敵が存在しない";
			finishEnemySequence();
			return;
		}

		auto& world = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
			.getPhysicsFactory()->getWorld();

		Vec2 hookPos = m_body->getPosition();
		auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
			.getPhysicsFactory()->getBody(m_ownerId);

		if (!playerBody)
		{
			finishEnemySequence();
			return;
		}

		Vec2 playerPos = playerBody->getPosition();
		double dist = (hookPos - playerPos).length();
		dist = std::clamp(dist, 1.0, 9999.0);

		releaseJoint();

		m_body->setBodyType(PhysicsType::Static);

		m_joint = m_body->createDistanceJoint(world, playerBody, hookPos, playerPos, dist);

		if (m_joint.has_value())
		{
			m_joint->setLinearStiffness(15.0, 0.8);
			m_joint->setMinLength(0.0);
			m_joint->setMaxLength(dist);

			Vec2 diff = hookPos - playerPos;
			if (diff.isZero()) diff = Vec2{ 0, -1 };
			m_lastDir = diff.normalized();

			m_isJointCreated = true;
		}
		else
		{
			Print << U"❌ 敵用ジョイント作成失敗";
			finishEnemySequence();
		}
	}

	bool ChokerSkill::isFlying() const { return m_isActive; }
	bool ChokerSkill::isOnCooldown() const { return m_cooldownTimer > 0.0; }

	double ChokerSkill::getCooldownProgress() const
	{
		if (m_cooldownTime <= 0.0) return 1.0;
		return 1.0 - (m_cooldownTimer / m_cooldownTime);
	}

	double ChokerSkill::getRemainingCooldown() const { return m_cooldownTimer; }
	double ChokerSkill::getHookedMoveSpeedMultiplier() const { return m_hookedMoveSpeed; }

	void ChokerSkill::onDeactivate()
	{
		if (m_joint.has_value())
		{
			resetHook();
			releaseJoint();
		}
	}

	ChokerSkill::~ChokerSkill()
	{
		releaseJoint();
	}
}
