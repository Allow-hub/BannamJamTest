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
				   Physics::PhysicsBodyID playerId)
			: m_rect(obj.rect)
			, m_damageAmount(obj.damageAmount)
			, m_body(body)
			, m_eventQueue(eventQueue)
			, m_playerId(playerId)
		{
		}

		/**
		 * 初期化メソッド
		 * コンストラクタ後、ステージがunique_ptrで管理された後に呼び出す必要がある
		 */
		void init() {
			if (!m_body) {
				return;
			}

			// 自身へのshared_ptrを作成（ライフタイム管理はunique_ptrが行う）
			// カスタムデリーター: 削除時に何もしない（unique_ptrが破棄を担当）
			auto noOpDeleter = [](DamageStage*) {
				// 何もしない: unique_ptrがオブジェクトの破棄を担当する
				};
			m_selfPtr = std::shared_ptr<DamageStage>(this, noOpDeleter);

			// ICollisionListenerとしてキャスト
			auto listener = std::dynamic_pointer_cast<Physics::ICollisionListener>(m_selfPtr);

			// 物理ボディに衝突リスナーを設定
			m_body->setCollisionListener(listener);
		}

		void update(double deltaTime) override {
			m_elapsedTime += deltaTime;
		}

		RectF getRenderRect() const override {
			return m_rect;
		}

		StageType getType() const override {
			return StageType::DamagePlatform;
		}

		Vec2 getCurrentCenter() const override {
			return m_rect.center();
		}

		// ICollisionListener実装
		void onCollisionEnter(std::shared_ptr<Physics::IPhysicsBody> other) override {
			handleCollision(other);
		}

		void onCollisionStay(std::shared_ptr<Physics::IPhysicsBody> other) override {
			handleCollision(other);
		}

		void onCollisionExit(std::shared_ptr<Physics::IPhysicsBody> other) override {}

	private:
		void handleCollision(std::shared_ptr<Physics::IPhysicsBody> other) {
			// プレイヤーとの衝突のみ処理
			if (other->getLayer() != Physics::PhysicsLayer::Player) {
				return;
			}

			// ダメージ間隔チェック
			if (m_elapsedTime - m_lastDamageTime < m_damageInterval) {
				return;
			}

			// ダメージイベントを発行
			m_eventQueue.push(Events::PlayerDamagedEvent{
				m_body->getID(),
				m_playerId,
				DamageInfo{
					m_damageAmount,
					m_body->getPosition(),
					Vec2(0, -1),  // 上向き
					false,  // クリティカルではない
					false   // 貫通しない
				},
				0.0,   // ヒットストップ時間
				0.3,   // 無敵時間
				15.0   // ノックバック力
			});

			m_lastDamageTime = m_elapsedTime;
		}
	};
}
