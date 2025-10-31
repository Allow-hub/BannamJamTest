#include "Bomb.h"
#include "../../Infrastructure/FactoryServiceLocator.h"
#include "../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../Infrastructure/PhysicsFilterManager.h"
#include "../../Infrastructure/IndependentObjectFactory.h"
#include "../../Presentation/AudioService.h"

using namespace Jam::Domain::Physics;
using namespace Jam::Infrastructure::Locator;

namespace Jam::Domain::Enemy
{
	Bomb::Bomb(std::shared_ptr<IPhysicsBody> body, PhysicsBodyID playerId, Jam::Domain::Events::GameEventQueue& queue, double damage, double explosionRadius, double lifetime, Vec2 size)
		: m_body(body)
		, m_playerId(playerId)
		, m_eventQueue(queue)
		, m_damage(damage)
		, m_exlosionRadius(explosionRadius)
		, m_lifetime(lifetime)
		, m_size(size)
	{
		m_timer = 0.0;
		m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Dynamic);
		m_body->setGravityScale(1.5);
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::EnemyWeapon);
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::Enemy);
		bombTex = Texture(U"Assets/Item/bomb.png");
	}

	void Bomb::init()
	{
		m_body->setCollisionListener(shared_from_this());
	}

	Bomb::~Bomb()
	{
		// デストラクタで物理ボディを破壊
		if (m_body)
		{
			auto factory = FactoryServiceLocator::instance().getPhysicsFactory();
			factory->removeBody(m_body->getID());
			m_body.reset();
		}
		//Jam::Infrastructure::IndependentObjectFactory::instance().removeObjectByPtr(this);
	}

	void Bomb::update(double dt)
	{
		m_timer += dt;

		if (!m_isExploding && m_timer >= m_lifetime)
		{
			explosion();
		}

		if (m_isExploding)
		{
			m_explodeTimer += dt;

			// 一定時間後に当たり判定を削除
			const double hitDuration = m_explodeDuration * hitRatio;
			if (m_explodeBody && m_explodeTimer >= hitDuration)
			{
				auto factory = FactoryServiceLocator::instance().getPhysicsFactory();
				factory->removeBody(m_explodeBody->getID());
				m_explodeBody.reset();
			}

			// 爆発全体が終わったら削除
			if (m_explodeTimer >= m_explodeDuration)
			{
				m_isDead = true;
			}
		}
	}


	void Bomb::draw() const
	{
		//if (!m_body) return;

		if (!m_isExploding)
		{
			if (m_body)
			{
				bombTex.scaled(m_size * m_scaled)
					.drawAt(m_body->getPosition());
			}
		}
	}

	void Bomb::explosion()
	{
		if (m_isExploding) return; // 二重処理防止

		auto physicsFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory();
		//P2BodyTypeの指定は後で修正
		m_explodeBody = physicsFactory->createCircleSensor(m_body->getPosition(), m_exlosionRadius);
		m_explodeBody->setCollisionListener(shared_from_this());

		//爆弾自体のボディを破壊
		if (m_body)
		{
			auto factory = FactoryServiceLocator::instance().getPhysicsFactory();
			factory->removeBody(m_body->getID());
			m_body.reset();
		}


		m_eventQueue.push(Events::ExplosionEvent{
			m_explodeBody->getPosition(),Palette::Red ,m_exlosionRadius,1.0,300
		});
		Jam::Presentation::AudioService::get().playOneShot(Jam::Presentation::AudioService::Sound::SE_Explosion, 0.5);

		m_isExploding = true;
		m_timer = m_lifetime; // 強制的に爆発状態に
		m_explodeTimer = 0.0;
	}


	void Bomb::onCollisionEnter(std::shared_ptr<IPhysicsBody> other)
	{
		if (other->getLayer() != Jam::Domain::Physics::PhysicsLayer::Player)return;
		if (!m_isExploding)
			explosion();//爆発状態でないときにぶつかったら即爆発
		else
		{
			if (m_isHit)return;
			//爆発用のボディに触れた時の実際にダメージを当たる処理
			m_eventQueue.push(Events::PlayerDamagedEvent{
				m_explodeBody->getID(),
				m_playerId,
				DamageInfo {
				m_damage,
				m_explodeBody->getPosition(),
				(Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory()->getBody(m_playerId)->getPosition()
				- m_explodeBody->getPosition()).normalized(),
				true,
				false
				}
				,0.0
				,0.3
				,15.0
			});
			m_isHit = true;
		}
	}

	void Bomb::onCollisionStay(std::shared_ptr<IPhysicsBody> other) {}
	void Bomb::onCollisionExit(std::shared_ptr<IPhysicsBody> other) {}
}
