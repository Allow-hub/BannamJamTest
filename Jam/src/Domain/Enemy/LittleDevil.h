#pragma once
#include "EnemyBase.h"

namespace Jam::Domain::Enemy
{
	// 小悪魔タイプの敵クラス
	// 飛行や短距離突進など、特徴的な挙動を今後追加予定
	class LittleDevil : public EnemyBase
	{
	public:
		explicit LittleDevil(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, std::shared_ptr<Jam::Domain::Player::Player>& player);
		virtual ~LittleDevil() = default;

		// 毎フレームの更新（AI挙動など）
		void update(double deltaTime) override;

		// 当たり判定イベント（必要に応じて上書き）
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override;

	private:
		// 将来的にAI状態管理を追加予定
		enum class State
		{
			Idle,
			Patrol,
			Attack,
			Dead
		};

		State m_state = State::Idle;
		double m_patrolTimer = 0.0;
	};
}
