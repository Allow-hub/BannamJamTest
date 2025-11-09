#pragma once
#include <memory>
#include "../Physics/IPhysicsBody.h"
#include "../Physics/ICollisionListener.h"
#include "../Events/GameEvents.h"
#include "../ITakeDamageable.h"
#include "../../Foundation/CoroutineUtil.h"
#include "../../Presentation/FadeManager.h"

namespace Jam::Domain::Player
{
	class ChokerSkill;

	struct PlayerStats
	{
		double hp;
		double power;
		double moveSpeed;
		double jumpPower;
		Jam::Domain::Physics::PhysicsMaterial physicsMaterial;
	};

	// プレイヤーキャラクターを表すクラス
	class Player : public Jam::Domain::Physics::ICollisionListener, public Jam::Domain::ITakeDamageable
	{
	public:
		explicit Player(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Events::GameEventQueue& eventQueue);

		// 更新・描画
		void update(double deltaTime);
		void draw() const;

		// 移動・操作
		void moveLeft(double dt);
		void moveRight(double dt);
		void startDash();
		void endDash();
		void jump();
		void setIsRespawning(bool b) { m_isRespawning = b; }
		bool getIsRespawning() { return m_isRespawning; }
		void setCanControl(bool b) { m_canControl = b; }
		bool getCanControl() { return m_canControl; }
		void controlCooldown(double cooldown);
		Jam::Util::Task controlCooldownProcess(double cooldown);

		bool getIsChokering()const { return m_isChokering; }

		// チョーカー操作
		void chokerPush();
		void chokerReleased();

		// 情報取得
		double getMaxHp() { return m_maxHp; }
		s3d::Vec2 getPosition() const;
		bool isFacingRight() const;
		std::shared_ptr<Domain::Physics::IPhysicsBody> getPhysicsBody() { return m_body; }

		// ステータス操作
		void setHp(double h) { m_stats.hp = h; }
		void setPower(double p) { m_stats.power = p; }
		void setSpeed(double s) { m_stats.moveSpeed = s; }
		void setJumpPower(double j) { m_stats.jumpPower = j; }
		void setMaxHp(double h) { m_maxHp = h; }

		double getHp() { return m_stats.hp; }
		bool getGrounded() const { return m_isGrounded; }

		void resetJumpState() {
			m_isGrounded = true;
			m_jumpCount = 0;
		}

		bool isPressingDown() const { return m_isPressingDown; }
		void setPressingDown(bool pressing) { m_isPressingDown = pressing; }

		bool isAlive() const override { return m_isAlive; }
		double getCurrentHp() const override { return m_stats.hp; }
		void takeDamage(const DamageInfo& info) override;
		bool setIsInvincible(bool b) { return m_isInvincible = b; }

		// ICollisionListener
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		using DamageCallback = std::function<void(void)>;
		void setOnDamagedCallback(DamageCallback callback);

	private:
		// 基本情報
		PlayerStats m_stats;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		double m_maxHp = 0.0;

		// 移動関連
		bool m_canControl = true;
		bool m_isGrounded = false;
		bool m_facingRight = true;
		bool m_isDashing = false;
		bool m_isChokering = false;
		bool m_isPressingDown = false;
		double m_fallLimitY = 0;
		int m_jumpCount = 0;
		const int maxJumpCount = 2;

		// チョーカー
		std::shared_ptr<ChokerSkill> m_choker;

		// 生存・ダメージ
		bool m_isAlive = true;
		bool m_isRespawning = false;
		bool m_isInvincible = false;
		void onDamaged(const DamageInfo& info);
		void onDeath();
		Jam::Util::Task respawn();

		DamageCallback m_onDamaged;
	};
}
