#pragma once
#include <memory>
#include "../Physics/IPhysicsBody.h"
#include "../Events/GameEvents.h"
#include "../Physics/ICollisionListener.h"
#include "../IIndependentObject.h"

namespace Jam::Domain::Enemy
{
	class Missile : public Jam::Domain::IIndependentObject
		, public Jam::Domain::Physics::ICollisionListener
		, public std::enable_shared_from_this<Missile>
	{
	private:
		Texture m_missileTex;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		Jam::Domain::Physics::PhysicsBodyID m_playerId;
		Jam::Domain::Physics::PhysicsBodyID m_bossId;

		// ベジェ曲線の制御点
		Vec2 m_controlPoint0; // 開始点(ボスの真後ろ)
		Vec2 m_controlPoint1; // 制御点1(ボスの上空)
		Vec2 m_controlPoint2; // 制御点2(プレイヤーの上空)
		Vec2 m_controlPoint3; // 終了点(プレイヤー位置)

		// ミサイルのパラメータ
		double m_damage;
		double m_flightDuration;      // 飛行時間(秒)
		double m_timer;                // 経過時間
		double m_radius;               // ミサイルの半径
		bool m_isReflected;            // 反射されたか
		Vec2 m_reflectedDirection;     // 反射後の方向
		double m_reflectedSpeed;       // 反射後の速度

		bool m_hasHitBoss;             // ボスに当たったか

	public:
		Missile(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
				Jam::Domain::Physics::PhysicsBodyID playerId,
				Jam::Domain::Physics::PhysicsBodyID bossId,
				Jam::Domain::Events::GameEventQueue& queue,
				Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3,
				double damage = 10.0,
				double flightDuration = 3.0,
				double radius = 15.0,
				double reflectedSpeed = 500.0);
		~Missile();

		void init();
		void update(double deltaTime) override;
		void draw() const override;

		// ベジェ曲線の計算
		Vec2 calculateBezierPoint(double t) const;

		// 反射処理
		void reflect(Vec2 direction);

		// 衝突検知
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
	};
}
