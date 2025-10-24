#pragma once
#include <memory>
#include "../Physics/IPhysicsBody.h"
#include "../Physics/ICollisionListener.h"
#include "../Events/GameEvents.h"
#include "Skill/IPlayerSkill.h"
#include "../ITakeDamageable.h"
#include "../../Foundation/CoroutineUtil.h"
#include "../../Presentation/FadeManager.h"

namespace Jam::Domain::Player
{
	class IPlayerSkill;
	enum class PlayerSkillType;

	struct PlayerStats
	{
		double hp;
		double power;
		double moveSpeed;
		double jumpPower;
		Jam::Domain::Physics::PhysicsMaterial physicsMaterial;
	};

	// プレイヤーキャラクターを表すクラス
	// 他クラスに依存しない
	class Player : public Jam::Domain::Physics::ICollisionListener, public Jam::Domain::ITakeDamageable
	{
	public:
		explicit Player(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Events::GameEventQueue& eventQueue);

		// 更新・描画
		void update(double deltaTime);
		void draw() const;

		// 移動・操作
		void moveLeft();
		void moveRight();
		void startDash();
		void endDash();
		void jump();
		void setIsRespawning(bool b) { m_isRespawning = b; }
		bool getIsRespawning() { return m_isRespawning; }
		void setCanControl(bool b) { m_canControl = b; }
		bool getCanControl() { return m_canControl; }
		void controlCooldown(double cooldown);
		Jam::Util::Task controlCooldownProcess(double cooldown);

		// 攻撃・スキル
		void skillPush();
		void skillReleased();
		void changeSkill(int direction);

		// 情報取得
		s3d::Vec2 getPosition() const;
		bool isFacingRight() const;
		std::shared_ptr<Domain::Physics::IPhysicsBody> getPhysicsBody() { return m_body; }
		double getHookedSpeedMultiplier() const;

		// ステータス操作
		void setHp(double h) { m_stats.hp = h; }
		void setPower(double p) { m_stats.power = p; }
		void setSpeed(double s) { m_stats.moveSpeed = s; }
		void setJumpPower(double j) { m_stats.jumpPower = j; }
		bool isAlive() const override { return m_isAlive; }
		double getCurrentHp() const override { return m_stats.hp; }
		void takeDamage(const DamageInfo& info) override;

		// ICollisionListener
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

	private:
		// 基本情報
		PlayerStats m_stats;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		Jam::Domain::Events::GameEventQueue& m_eventQueue;

		// 移動関連
		bool m_canControl = true;
		bool m_isGrounded = false;
		bool m_facingRight = true;
		bool m_isDashing = false;
		double dashMagnification = 2.0;
		double m_fallLimitY = 0;
		int m_jumpCount = 0;
		const int maxJumpCount = 2;

		// スキル
		std::vector<std::shared_ptr<IPlayerSkill>> m_skills;
		std::shared_ptr<IPlayerSkill> m_currentSkill;

		// 生存・ダメージ
		bool m_isAlive = true; // 生存フラグ
		bool m_isRespawning = false;
		void onDamaged(const DamageInfo& info);
		void onDeath();
		Jam::Util::Task respawn();

		// 内部処理
		void updateState();
	};
}
