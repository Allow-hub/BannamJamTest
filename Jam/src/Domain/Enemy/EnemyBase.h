#pragma once
#include "../Physics/ICollisionListener.h"
#include "../Physics/PhysicsTypes.h"

namespace Jam::Domain::Enemy
{
	struct EnemyStatus
	{
		int hp;
		double attackPower;
		double moveSpeed;
		double jumpPower;
		Jam::Domain::Physics::PhysicsMaterial physicsMaterial;
	};

	// 敵の基底クラス（AIや個別挙動は派生クラスで実装）
	class EnemyBase : public Jam::Domain::Physics::ICollisionListener
	{
	public:
		explicit EnemyBase(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body);
		virtual ~EnemyBase() = default;

		virtual void update(double deltaTime) = 0; // 各敵のAI・行動

		virtual void moveLeft();
		virtual void moveRight();
		virtual void jump();

		virtual void onDestroy();

		// HP 管理
		bool isAlive() const { return m_isAlive; }
		void takeDamage(int damage);

		// 位置取得
		s3d::Vec2 getPosition() const;
		void setPos(Vec2 p)const;
		void setGravityScale(double s)const;

		// 状態設定
		virtual void setStatus(const EnemyStatus& status) { m_status = status; }
		const EnemyStatus& getStatus() const { return m_status; }

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> getPhysicsBody() { return m_body; }

		// ICollisionListener 実装
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

	protected:
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		EnemyStatus m_status;
		bool m_isAlive = true;
	};
}
