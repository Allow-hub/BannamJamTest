#include "EnemyBase.h"
#include "../Physics/IPhysicsBody.h"
#include "../Physics/PhysicsBodyID.h"
#include "EnemyAI/IEnemyAI.h"
#include "../../Infrastructure/FactoryServiceLocator.h"
#include "../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../Infrastructure/PhysicsFilterManager.h"
#include "../Events/GameEvents.h"


namespace Jam::Domain::Enemy
{
	EnemyBase::EnemyBase(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId, Jam::Domain::Events::GameEventQueue& eventQueue)
		: m_body(body)
		, m_playerId(playerId)
		, m_eventQueue(eventQueue)
		, m_isAlive(true)
	{
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::Team2);
		m_body->setGravityScale(1);
		m_body->setDamping(2);
		m_body->setBullet(true);
		m_body->setSleepEnabled(false);
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

	Vec2 reflect(const s3d::Vec2& dir, const s3d::Vec2& normal)
	{
		return dir - 2 * dir.dot(normal) * normal;
	}

	Jam::Util::Task EnemyBase::onDeath(const DamageInfo& info)
	{
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::Team2Death);
		m_body->setGravityScale(0);
		Vec2 impulseDir = info.direction;
		m_isDeadAttack = true;
		if (m_isGrounded && info.direction.y > 0) // 下向き
		{
			Vec2 floorNormal{ 0, -1 };
			impulseDir = reflect(info.direction, floorNormal);
		}
		m_enemyImpluseDir = impulseDir;
		m_body->setDamping(0);
		m_body->applyImpulse(impulseDir * 8000);
		co_await Jam::Util::WaitSeconds(1.0);
		onDestroy(info);
	}

	void EnemyBase::onDestroy(const DamageInfo& info)
	{
		if (!info.isEnemyCombo)
			m_eventQueue.push(Events::EnemyDefeatedEvent{
				m_body->getPosition(),
				false,
				m_enemyType,
				0.88,
				0.5,
				10
			});
		Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
			.getPhysicsFactory()->removeBody(m_body->getID());
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

	//敵の攻撃イベントの送信は具象クラス側に任せます
	void EnemyBase::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		switch (other->getLayer())
		{
		case Physics::PhysicsLayer::Ground:
			m_isGrounded = true;
			break;
		case Physics::PhysicsLayer::Wall:
			// 壁との衝突 - 地上判定はしない
			break;
		case Physics::PhysicsLayer::Enemy:
			if (m_isDeadAttack)
			{
				m_eventQueue.push(Events::EnemyDamagedEvent{
					m_body->getID(),
					other->getID(),
					DamageInfo {
						100,//power
						m_body->getPosition(),
						{0,0},
						true,
						true
					}
				});
				m_body->setVelocity({ 0,0 });
				onDestroy({ 0.0,{ 0,0 },{ 0,0 },false,false });
			}
			break;
		default:
			break;
		}
	}

	void EnemyBase::draw() const
	{
	}

	void EnemyBase::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}
	void EnemyBase::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		switch (other->getLayer())
		{
		case Physics::PhysicsLayer::Ground:
			m_isGrounded = false;
			break;
		case Physics::PhysicsLayer::Wall:
			// 壁との衝突終了 - 何もしない
			break;
		default:
			break;
		}
	}
}
