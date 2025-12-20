#pragma once
#include <memory>
#include "Domain/Physics/IPhysicsBody.h"
#include "Domain/Events/GameEvents.h"
#include "Domain/Physics/ICollisionListener.h"
#include "Domain/IIndependentObject.h"
#include "Domain/Player/StatusAilment.h"

namespace Jam::Domain::Enemy
{
	// 毒の弾
	// 毒の植物が放つ、プレイヤーに数秒間ホーミングする弾
	class PoisonBullet : public Jam::Domain::IIndependentObject
		, public Jam::Domain::Physics::ICollisionListener
		, public std::enable_shared_from_this<PoisonBullet>
	{
	private:
		Texture fbTex;

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		Jam::Domain::Physics::PhysicsBodyID m_playerId;

		double m_damage;
		double m_lifetime; // 弾自体の寿命
		double m_homingTime; // ホーミングする時間
		double m_timer; // 生存時間計測
		double m_homingTimer; // ホーミング時間計測

		Vec2 m_size;
		Vec2 m_scaled = Vec2{0.0015 ,0.0015 };

		Vec2 m_Velocity;

		bool m_isHit = false;

		// ホーミング状態
		enum class HomingState
		{
			Homing, // ホーミング中
			Straight, //直進のみ
		};

		HomingState m_state = HomingState::Homing;

		// 命中時に付与する状態異常
		Jam::Domain::Player::StatusAilmentType m_statusType{ Jam::Domain::Player::StatusAilmentType::Poison };
		double m_statusDuration{0.0 };
		double m_statusPower{0.0 };
		double m_statusTickInterval{0.0 };

	public:
		PoisonBullet(
			std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
			Jam::Domain::Physics::PhysicsBodyID playerId,
			Jam::Domain::Events::GameEventQueue& queue,
			double damage,
			double lifetime,
			double homingTime,
			Vec2 size,
			Vec2 Velocity
		);

		// 状態異常を指定する用のセッター
		void setStatusAilment(Jam::Domain::Player::StatusAilmentType type,
			double duration,
			double power,
			double tickInterval = 0.0);

		~PoisonBullet();

		void init();
		void update(double deltaTime) override;
		void draw() const override;

		// 衝突検知
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
	};
}
