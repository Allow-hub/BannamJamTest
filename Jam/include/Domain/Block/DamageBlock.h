#pragma once
#include "Domain/Stage/IStage.h"
#include "Domain/Physics/ICollisionListener.h"
#include "Domain/Physics/IPhysicsBody.h"
#include "Domain/Events/GameEvents.h"
#include <memory>

namespace Jam::Domain::Stage {
	/**
	 * ダメージを与える床
	 * プレイヤーが触れるとダメージを受ける静的なステージオブジェクト
	 */
	class DamageStage : public IStage
		, public Physics::ICollisionListener {
	private:
		RectF m_rect;
		double m_damageAmount;  // 与えるダメージ量
		std::shared_ptr<Physics::IPhysicsBody> m_body;  // メイン物理ボディ
		Events::GameEventQueue& m_eventQueue;  // イベントキュー
		Physics::PhysicsBodyID m_playerId;  // プレイヤーID

		// 自身へのshared_ptr（ライフタイム管理用）
		std::shared_ptr<DamageStage> m_selfPtr;

		// ダメージ間隔管理
		double m_damageInterval = 0.5;  // ダメージを与える間隔（秒）
		double m_lastDamageTime = -999.0;  // 最後にダメージを与えた時刻
		double m_elapsedTime = 0.0;  // 経過時間

	public:
		DamageStage(const StageObject& obj,
				   std::shared_ptr<Physics::IPhysicsBody> body,
				   Events::GameEventQueue& eventQueue,
				   Physics::PhysicsBodyID playerId);

		/**
		 * 初期化メソッド
		 * コンストラクタ後、ステージがunique_ptrで管理された後に呼び出す必要がある
		 */
		void init();

		void update(double deltaTime) override;

		RectF getRenderRect() const override;

		StageType getType() const override;

		Vec2 getCurrentCenter() const override;

		// ICollisionListener実装
		void onCollisionEnter(std::shared_ptr<Physics::IPhysicsBody> other) override;

		void onCollisionStay(std::shared_ptr<Physics::IPhysicsBody> other) override;

		void onCollisionExit(std::shared_ptr<Physics::IPhysicsBody> other) override;

	private:
		void handleCollision(std::shared_ptr<Physics::IPhysicsBody> other);
	};
}
