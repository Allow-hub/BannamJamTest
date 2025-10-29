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
		, m_normalSpeed(0.0)
		, m_hasHitBoss(false)
	{
		// PhysicsBodyの設定
		m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Dynamic);
		m_body->setGravityScale(0.0); // ミサイルは重力の影響を受けない
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::EnemyWeapon);
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::Enemy);

		// 通常速度を計算(ベジェ曲線の大まかな長さ / 飛行時間)
		double approximateLength = (m_controlPoint1 - m_controlPoint0).length() +
								   (m_controlPoint2 - m_controlPoint1).length() +
								   (m_controlPoint3 - m_controlPoint2).length();
		m_normalSpeed = approximateLength / m_flightDuration;
	}

	void Missile::init()
	{
		m_body->setCollisionListener(shared_from_this());
		Print << U"[Missile] Created! Body Layer: " << Jam::Domain::Physics::ToString(m_body->getLayer());
		Print << U"[Missile] Position: " << m_body->getPosition();
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

		// 仮描画: 円
		Circle(m_body->getPosition(), m_radius).draw(Palette::Orange);
		
		// 進行方向を示す線
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
		
		Line(m_body->getPosition(), m_body->getPosition() + direction * m_radius * 2).draw(3, Palette::Red);
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

		Print << U"[Missile] 🔄 Reflecting! Direction: " << direction;
		m_isReflected = true;
		m_reflectedDirection = direction.normalized();
		
		// 反射時のフィルター変更(プレイヤーの武器扱いに)
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::Team1);
		m_body->setLayer(Jam::Domain::Physics::PhysicsLayer::ReflectableWeapon);
		Print << U"[Missile] New Layer: " << Jam::Domain::Physics::ToString(m_body->getLayer());
	}

	void Missile::onCollisionEnter(std::shared_ptr<IPhysicsBody> other)
	{
		Print << U"[Missile] Collision detected! Other layer: " << Jam::Domain::Physics::ToString(other->getLayer());
		Print << U"[Missile] IsReflected: " << m_isReflected;
		
		// チョーカーとの衝突(反射)
		if (!m_isReflected && other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Weapon)
		{
			Print << U"[Missile] ✅ Reflected by Choker!";
			// プレイヤーからミサイルへの方向の逆向き = 反射方向
			Vec2 reflectDir = (m_body->getPosition() - other->getPosition()).normalized();
			reflect(reflectDir);
			return;
		}

		// 反射後のボスとの衝突
		if (m_isReflected && !m_hasHitBoss && other->getID() == m_bossId)
		{
			Print << U"[Missile] ✅ Hit Boss after reflection!";
			m_hasHitBoss = true;
			
			// ボスにダメージを与える
			m_eventQueue.push(Events::EnemyDamagedEvent{
				m_body->getID(),
				m_bossId,
				DamageInfo{
					m_damage,
					m_body->getPosition(),
					m_reflectedDirection,
					false,
					false
				}
			});

			// ミサイル削除
			m_isDead = true;
			return;
		}

		// プレイヤーとの衝突(反射されていない場合)
		if (!m_isReflected && other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Player)
		{
			Print << U"[Missile] ✅ Hit Player directly!";
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