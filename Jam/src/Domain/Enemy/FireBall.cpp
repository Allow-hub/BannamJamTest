#include "Fireball.h"
#include "../../Infrastructure/FactoryServiceLocator.h"
#include "../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../Infrastructure/PhysicsFilterManager.h"
#include "../../Presentation/AudioService.h"

using namespace Jam::Domain::Physics;
using namespace Jam::Infrastructure::Locator;

namespace Jam::Domain::Enemy
{
	Fireball::Fireball(
		std::shared_ptr<IPhysicsBody> body,
		PhysicsBodyID playerId,
		Jam::Domain::Events::GameEventQueue& queue,
		double damage,
		double lifetime,
		Vec2 size,
		Vec2 initialVelocity
	)
		: m_body(body)
		, m_playerId(playerId)
		, m_eventQueue(queue)
		, m_damage(damage)
		, m_lifetime(lifetime)
		, m_size(size)
		, m_initialVelocity(initialVelocity)
	{
		m_timer = 0.0;

		m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Dynamic);

		m_body->setGravityScale(0.0);

		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::EnemyWeapon);
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::Enemy);

		m_texture = Texture(Resource(U"Assets/Item/fireBall.png"));
	}

	void Fireball::init()
	{
		m_body->setCollisionListener(shared_from_this());
		m_body->applyImpulse(m_initialVelocity);
	}

	Fireball::~Fireball()
	{
		if (m_body)
		{
			auto factory = FactoryServiceLocator::instance().getPhysicsFactory();
			factory->removeBody(m_body->getID());
			m_body.reset();
		}
	}

	void Fireball::update(double dt)
	{
		m_timer += dt;

		m_body->applyForce(m_initialVelocity * flySpeed);

		// 一定時間経過したら自然消滅
		if (m_timer >= m_lifetime)
		{
			m_isDead = true;
		}
	}

	/*
	void Fireball::draw() const
	{
		if (m_body)
		{
			m_texture.scaled(m_size * m_scaled)
				.drawAt(m_body->getPosition());
		}
	}
	*/
	void Fireball::draw() const
	{
		if (!m_body) return;

		// テクスチャ未読み込み時は明示的に目印を描画（黄色いサークル）
		if (!m_texture)
		{
			Circle(m_body->getPosition(), 12).draw(Palette::Yellow);
			return;
		}

		// テクスチャ実サイズを取得してスケールを計算（m_size は表示したいピクセルサイズ）
		const Vec2 texSize = m_texture.size();
		if (texSize.x <= 0 || texSize.y <= 0) return;

		const Vec2 scale = m_size / texSize;
		m_texture.scaled(scale).drawAt(m_body->getPosition());
	}
	void Fireball::onCollisionEnter(std::shared_ptr<IPhysicsBody> other)
	{
		if (m_isHit) return;

		// プレイヤーに当たった場合
		if (other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Player)
		{
			m_eventQueue.push(Events::PlayerDamagedEvent{
				m_body->getID(),
				m_playerId,
				DamageInfo {
					m_damage,
					m_body->getPosition(),
					m_initialVelocity.normalized(),
					true,
					false
				},
				0.0,
				0.3,
				10.0 // ノックバック力
			});

			m_isHit = true;
			m_isDead = true; // 自身は消滅
		}
		// 壁や地面に当たった場合も消滅させる
		else if (other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Ground ||
				 other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Wall)
		{
			m_isHit = true;
			m_isDead = true;
		}
	}

	void Fireball::onCollisionStay(std::shared_ptr<IPhysicsBody> other) {}
	void Fireball::onCollisionExit(std::shared_ptr<IPhysicsBody> other) {}
}
