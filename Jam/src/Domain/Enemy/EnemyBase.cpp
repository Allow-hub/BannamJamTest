#include "EnemyBase.h"
#include "../Physics/IPhysicsBody.h"
#include "../Physics/PhysicsBodyID.h"
#include "EnemyAI/IEnemyAI.h"
#include "../../Infrastructure/FactoryServiceLocator.h"
#include "../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../Infrastructure/PhysicsFilterManager.h"

namespace Jam::Domain::Enemy
{
	EnemyBase::EnemyBase(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId)
		: m_body(body)
		, m_playerId(playerId)	
		, m_isAlive(true)
	{
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::Team2);
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

	void EnemyBase::takeDamage(const DamageInfo& info)
	{
		if (!m_isAlive) return;

		m_status.hp -= info.amount;
		Print << m_status.hp;
		if (m_status.hp <= 0)
		{
			m_status.hp = 0;
			onDeath(info); // 死んだとき
		}
		else
		{
			//onDamaged(info);//ダメージを受けた時の吹き飛ばし等
		}
	}

	Jam::Util::Task EnemyBase::onDeath(const DamageInfo& info)
	{
		m_body->applyImpulse(info.direction*100);
		co_await Jam::Util::WaitSeconds(1.0);
		Print << U"Hello!OneSeconds";
		co_await Jam::Util::WaitSeconds(5.0);
		Print << U"Hello!TwoSeconds";
		m_isAlive = false;
	}

	s3d::Vec2 EnemyBase::getPosition() const
	{
		return m_body ? m_body->getPosition() : s3d::Vec2{ 0, 0 };
	}
	void EnemyBase::setPos(Vec2 p)
	{
		m_body->setPos(p);
	}

	s3d::Vec2 EnemyBase::getPlayerPos()
	{
		auto physicsFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory();
		if (!physicsFactory) return s3d::Vec2{ 0,0 };

		auto playerBody = physicsFactory->getBody(m_playerId);
		if (!playerBody) return s3d::Vec2{ 0,0 };

		return playerBody->getPosition();
	}
	void EnemyBase::setGravityScale(double s)
	{
		m_body->setGravityScale(s);
	}

	//AIのリストを設定
	void EnemyBase::setAIList(std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList)
	{
		m_aiList = std::move(aiList);
		if (!m_aiList.empty())
		{
			m_currentAI = m_aiList.front().second.get();
			m_currentAI->enter(*this);
		}
	}

	//AIを切り替え
	void EnemyBase::changeAI(AIType type)
	{
		for (auto& [aiType, ai] : m_aiList)
		{
			if (aiType == type)
			{
				if (m_currentAI == ai.get()) return;

				if (m_currentAI) m_currentAI->exit(*this);
				m_currentAI = ai.get();
				m_currentAI->enter(*this);
				return;
			}
		}
	}

	AIType EnemyBase::getAIType() const
	{
		for (const auto& [type, ai] : m_aiList)
		{
			if (ai.get() == m_currentAI)
				return type;
		}
		return AIType::None;
	}

	void EnemyBase::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		// プレイヤーや弾丸との接触など（必要に応じて派生クラスで上書き）
	}

	void EnemyBase::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void EnemyBase::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
}
