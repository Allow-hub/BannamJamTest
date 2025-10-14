#pragma once
#include "../Physics/ICollisionListener.h"
#include "../Physics/PhysicsTypes.h"
#include <functional>

namespace Jam::Domain::Enemy
{
	struct EnemyStatus
	{
		int hp;
		double attackPower;
		double moveSpeed;
		double jumpPower;
		SizeF colSize;
		Jam::Domain::Physics::PhysicsMaterial physicsMaterial;
	};

	class EnemyBase : public Jam::Domain::Physics::ICollisionListener
	{
	public:
		using AnimationEvent = std::function<void(const s3d::String&)>;

		explicit EnemyBase(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body);
		virtual ~EnemyBase() = default;

		virtual void update(double deltaTime) = 0;
		virtual void moveLeft();
		virtual void moveRight();
		virtual void jump();
		virtual void onDestroy();

		bool isAlive() const { return m_isAlive; }
		void takeDamage(int damage);

		s3d::Vec2 getPosition() const;
		void setPos(s3d::Vec2 p);
		void setGravityScale(double s);

		void setStatus(const EnemyStatus& status) { m_status = status; }
		const EnemyStatus& getStatus() const { return m_status; }

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> getPhysicsBody() { return m_body; }

		// 衝突イベント
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		// アニメーション通知登録
		void setOnAnimationChange(AnimationEvent callback) { m_onAnimChange = std::move(callback); }

	protected:
		void changeAnimation(const s3d::String& animName)
		{
			if (m_onAnimChange)
				m_onAnimChange(animName);
		}

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		EnemyStatus m_status;
		bool m_isAlive = true;
		AnimationEvent m_onAnimChange;
	};
}
