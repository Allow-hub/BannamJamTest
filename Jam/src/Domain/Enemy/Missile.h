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

		// 繝吶ず繧ｧ譖ｲ邱壹・蛻ｶ蠕｡轤ｹ
		Vec2 m_controlPoint0; // 髢句ｧ狗せ(繝懊せ縺ｮ逵溷ｾ後ｍ)
		Vec2 m_controlPoint1; // 蛻ｶ蠕｡轤ｹ1(繝懊せ縺ｮ荳顔ｩｺ)
		Vec2 m_controlPoint2; // 蛻ｶ蠕｡轤ｹ2(繝励Ξ繧､繝､繝ｼ縺ｮ荳顔ｩｺ)
		Vec2 m_controlPoint3; // 邨ゆｺ・せ(繝励Ξ繧､繝､繝ｼ菴咲ｽｮ)

		// 繝溘し繧､繝ｫ縺ｮ繝代Λ繝｡繝ｼ繧ｿ
		double m_damage;
		double m_flightDuration;      // 鬟幄｡梧凾髢・遘・
		double m_timer;                // 邨碁℃譎る俣
		double m_radius;               // 繝溘し繧､繝ｫ縺ｮ蜊雁ｾ・
		double m_scale;                // 謠冗判繧ｹ繧ｱ繝ｼ繝ｫ菫よ焚
		bool m_isReflected;            // 蜿榊ｰ・＆繧後◆縺・
		Vec2 m_reflectedDirection;     // 蜿榊ｰ・ｾ後・譁ｹ蜷・
		double m_reflectedSpeed;       // 蜿榊ｰ・ｾ後・騾溷ｺｦ

		bool m_hasHitBoss;             // 繝懊せ縺ｫ蠖薙◆縺｣縺溘°

	public:
		Missile(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
				Jam::Domain::Physics::PhysicsBodyID playerId,
				Jam::Domain::Physics::PhysicsBodyID bossId,
				Jam::Domain::Events::GameEventQueue& queue,
				Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3,
				double damage = 10.0,
				double flightDuration = 3.0,
				double radius = 15.0,
				double reflectedSpeed = 500.0,
				double scale = 3.5);
		~Missile();

		void init();
		void update(double deltaTime) override;
		void draw() const override;

		// 繝吶ず繧ｧ譖ｲ邱壹・險育ｮ・
		Vec2 calculateBezierPoint(double t) const;

		// 蜿榊ｰ・・逅・
		void reflect(Vec2 direction);

		// 陦晉ｪ∵､懃衍
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
	};
}
