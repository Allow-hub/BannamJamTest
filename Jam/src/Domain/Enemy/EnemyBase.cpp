#include "EnemyBase.h"
#include "../Physics/IPhysicsBody.h"

namespace Jam::Domain::Enemy
{
	EnemyBase::EnemyBase(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body)
		: m_body(std::move(body))
		, m_isAlive(true)
	{
	}

	void EnemyBase::moveLeft()
	{
		if (!m_isAlive) return;
		m_body->applyForce({ -m_status.moveSpeed, 0 });
	}

	void EnemyBase::moveRight()
	{
		if (!m_isAlive) return;
		m_body->applyForce({ m_status.moveSpeed, 0 });
	}

	void EnemyBase::jump()
	{
		if (!m_isAlive) return;
		m_body->applyForce({ 0, -m_status.jumpPower });
	}

	void EnemyBase::takeDamage(int damage)
	{
		m_status.hp -= damage;
		if (m_status.hp <= 0)
		{
			m_isAlive = false;
			onDestroy();
		}
	}

	s3d::Vec2 EnemyBase::getPosition() const
	{
		return m_body ? m_body->getPosition() : s3d::Vec2{ 0, 0 };
	}
	void EnemyBase::setPos(Vec2 p) const
	{
		m_body->setPos(p);
	}

	void EnemyBase::setGravityScale(double s) const
	{
		m_body->setGravityScale(s);
	}

	void EnemyBase::onDestroy()
	{
		// 共通破壊処理（派生クラスでエフェクトやスコア加算などを上書き可能）
	}

	void EnemyBase::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// プレイヤーや弾丸との接触など（必要に応じて派生クラスで上書き）
	}

	void EnemyBase::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void EnemyBase::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
}
