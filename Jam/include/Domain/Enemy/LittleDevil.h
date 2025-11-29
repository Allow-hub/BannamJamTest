#pragma once
#include "EnemyBase.h"
#include "../Events/GameEvents.h"

namespace Jam::Domain::Enemy
{
	// 小悪魔タイプの敵クラス
	// 飛行や短距離突進など、特徴的な挙動を今後追加予定
	class LittleDevil : public EnemyBase
	{
	public:
		explicit LittleDevil(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
		, Jam::Domain::Events::GameEventQueue& eventQueue);
		virtual ~LittleDevil() = default;

		// 毎フレームの更新（AI挙動など）
		void update(double deltaTime) override;
		void draw() const override {};

		// 当たり判定イベント（必要に応じて上書き）
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

		void onAIEvent(EnemyAIEvent e) override;

	private:
		double m_patrolTimer = 0.0;
	};
}
