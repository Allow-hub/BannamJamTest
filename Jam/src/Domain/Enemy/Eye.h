#pragma once
#include "EnemyBase.h"
#include "../Events/GameEvents.h"
#include "EyeBeam.h"

namespace Jam::Domain::Enemy
{
	// 目の敵
	// プレイヤーに近づき、射程距離に入ると、目から長いビームを出す
	class Eye : public EnemyBase
	{
	public:
		explicit Eye(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
		, Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~Eye() = default;

		// 毎フレームの更新（AI挙動など）
		void update(double deltaTime) override;
		void draw() const override;
		// 当たり判定イベント
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		void onAIEvent(EnemyAIEvent e) override;

		void onDestroy(const DamageInfo& info)override;

	private :
		std::shared_ptr<EyeBeam> m_beamListener;

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_beam;
		bool m_attacked = false;
		bool m_rightMove = false;
		double attackTimer;
		double swingTimer;          // 揺れ動作用のタイマー
		double waitDuration = 4.0;  // 攻撃前の待機時間
		double attackDuration = 5.0; // 攻撃継続時間
		double swingInterval = 0.3;  // 左右揺れの間隔（秒）

		enum class AttackState
		{
			AttackStart = 0,
			WaitAttack,
			IsAttack,
			EndAttack
		};
		AttackState attackState;

	protected:
		void onAttackEnter()override;
		void onAttackUpdate(double deltaTime) override;
	};
}
