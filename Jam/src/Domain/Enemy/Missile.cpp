#include "Missile.h"
#include "../../Infrastructure/FactoryServiceLocator.h"
#include "../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../Infrastructure/PhysicsFilterManager.h"
#include "../../Infrastructure/IndependentObjectFactory.h"
#include "../../Presentation/AudioService.h"

using namespace Jam::Domain::Physics;
using namespace Jam::Infrastructure::Locator;

namespace Jam::Domain::Enemy
{
	Missile::Missile(std::shared_ptr<IPhysicsBody> body,
					 PhysicsBodyID playerId,
					 PhysicsBodyID bossId,
					 Jam::Domain::Events::GameEventQueue& queue,
					 Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3,
					 double damage,
					 double flightDuration,
					 double radius,
					 double reflectedSpeed)
		: m_body(body)
		, m_playerId(playerId)
		, m_bossId(bossId)
		, m_eventQueue(queue)
		, m_controlPoint0(p0)
		, m_controlPoint1(p1)
		, m_controlPoint2(p2)
		, m_controlPoint3(p3)
		, m_damage(damage)
		, m_flightDuration(flightDuration)
		, m_radius(radius)
		, m_timer(0.0)
		, m_isReflected(false)
		, m_reflectedDirection(Vec2::Zero())
		, m_reflectedSpeed(reflectedSpeed)
		, m_hasHitBoss(false)
	{
		// PhysicsBodyの設定
		m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Dynamic);
		m_body->setGravityScale(0.0); // ミサイルは重力の影響を受けない
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::Team2);
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::ReflectableWeapon);
		
		// ミサイルテクスチャの読み込み
		m_missileTex = Texture(U"../Assets/Item/Missile.png");
	}

	void Missile::init()
	{
		m_body->setCollisionListener(shared_from_this());
	}

	Missile::~Missile()
	{
		// デストラクタで物理ボディを破壊
		if (m_body)
		{
			auto factory = FactoryServiceLocator::instance().getPhysicsFactory();
			factory->removeBody(m_body->getID());
			m_body.reset();
		}
		Jam::Infrastructure::IndependentObjectFactory::instance().removeObjectByPtr(this);
	}

	void Missile::update(double dt)
	{
		if (!m_body) return;

		m_timer += dt;

		if (!m_isReflected)
		{
			// ベジェ曲線に沿って移動
			double t = Math::Min(m_timer / m_flightDuration, 1.0);
			Vec2 targetPos = calculateBezierPoint(t);
			m_body->setPos(targetPos);

			// 飛行時間を超えたら削除
			if (t >= 1.0)
			{
				m_isDead = true;
			}
		}
		else
		{
			// 反射後は直進
			Vec2 velocity = m_reflectedDirection * m_reflectedSpeed;
			Vec2 newPos = m_body->getPosition() + velocity * dt;
			m_body->setPos(newPos);

			// 画面外に出たら削除(簡易的な判定)
			if (newPos.x < -1000 || newPos.x > 3000 || newPos.y < -1000 || newPos.y > 3000)
			{
				m_isDead = true;
			}
		}
	}

	void Missile::draw() const
	{
		if (!m_body) return;

		// 進行方向の計算
		Vec2 direction;
		
		if (!m_isReflected)
		{
			// ベジェ曲線の接線方向
			double t = Math::Min(m_timer / m_flightDuration, 1.0);
			double epsilon = 0.01;
			Vec2 p1 = calculateBezierPoint(t);
			Vec2 p2 = calculateBezierPoint(Math::Min(t + epsilon, 1.0));
			direction = (p2 - p1).normalized();
		}
		else
		{
			direction = m_reflectedDirection;
		}
		
		// 方向から角度を計算
		// 画像が左向き(180度)なので、進行方向に合わせて補正
		double angle = Math::Atan2(direction.y, direction.x) + Math::Pi;
		
		// 画像のスケールを調整
		double scale = (m_radius * 2.5) / m_missileTex.width();
		
		// 画像を回転して描画
		m_missileTex.scaled(scale)
			.rotated(angle)
			.drawAt(m_body->getPosition());
	}

	Vec2 Missile::calculateBezierPoint(double t) const
	{
		// 3次ベジェ曲線の計算
		double u = 1.0 - t;
		double tt = t * t;
		double uu = u * u;
		double uuu = uu * u;
		double ttt = tt * t;

		Vec2 point = uuu * m_controlPoint0;              // (1-t)^3 * P0
		point += 3 * uu * t * m_controlPoint1;           // 3(1-t)^2 * t * P1
		point += 3 * u * tt * m_controlPoint2;           // 3(1-t) * t^2 * P2
		point += ttt * m_controlPoint3;                  // t^3 * P3

		return point;
	}

	void Missile::reflect(Vec2 direction)
	{
		if (m_isReflected) return; // 二重反射防止

		m_isReflected = true;
		m_reflectedDirection = direction.normalized();
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::Team1);
		// 反射後はLayerをWeaponに変更(ボス側で反射済みと判定するため)
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::Weapon);
	}

	void Missile::onCollisionEnter(std::shared_ptr<IPhysicsBody> other)
	{

		// チョーカーとの衝突(反射)
		if (!m_isReflected && other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Weapon)
		{
			Vec2 reflectDir = (m_body->getPosition() - other->getPosition()).normalized();
			reflect(reflectDir);
			return;
		}

		// 反射後のボスとの衝突(弱点露出のみ、ダメージなし)
		if (m_isReflected && !m_hasHitBoss && other->getID() == m_bossId)
		{
			m_hasHitBoss = true;
			
			// ボス側のonCollisionEnterで弱点露出フラグが設定される

			// ミサイル削除
			m_isDead = true;
			return;
		}

		// 反射前にボスに当たった場合(何もせず消滅)
		if (!m_isReflected && other->getID() == m_bossId)
		{
			m_isDead = true;
			return;
		}

		// プレイヤーとの衝突(反射されていない場合のみダメージ)
		if (!m_isReflected && other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Player)
		{
			m_eventQueue.push(Events::PlayerDamagedEvent{
				m_body->getID(),
				m_playerId,
				DamageInfo{
					m_damage,
					m_body->getPosition(),
					(other->getPosition() - m_body->getPosition()).normalized(),
					true,
					false
				},
				0.0,
				0.3,
				15.0
			});

			m_isDead = true;
		}
	}

	void Missile::onCollisionStay(std::shared_ptr<IPhysicsBody> other) {}
	void Missile::onCollisionExit(std::shared_ptr<IPhysicsBody> other) {}
}