#include "Domain/Block/DamageBlock.h"

namespace Jam::Domain::Block {
	DamageBlock::DamageBlock(const BlockObject& obj,
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

	void DamageBlock::init() {
		if (!m_body) {
			return;
		}

		// 自身へのshared_ptrを作成（ライフタイム管理はunique_ptrが行う）
		// カスタムデリーター: 削除時に何もしない（unique_ptrが破棄を担当）
		auto noOpDeleter = [](DamageBlock*) {
			// 何もしない: unique_ptrがオブジェクトの破棄を担当する
			};
		m_selfPtr = std::shared_ptr<DamageBlock>(this, noOpDeleter);

		// ICollisionListenerとしてキャスト
		auto listener = std::dynamic_pointer_cast<Physics::ICollisionListener>(m_selfPtr);

		// 物理ボディに衝突リスナーを設定
		m_body->setCollisionListener(listener);
	}

	void DamageBlock::update(double deltaTime) {
		m_elapsedTime += deltaTime;
	}

	RectF DamageBlock::getRenderRect() const {
		return m_rect;
	}

	BlockType DamageBlock::getType() const {
		return BlockType::DamagePlatform;
	}

	Vec2 DamageBlock::getCurrentCenter() const {
		return m_rect.center();
	}

	// ICollisionListener実装
	void DamageBlock::onCollisionEnter(std::shared_ptr<Physics::IPhysicsBody> other) {
		handleCollision(other);
	}

	void DamageBlock::onCollisionStay(std::shared_ptr<Physics::IPhysicsBody> other) {
		handleCollision(other);
	}

	void DamageBlock::onCollisionExit(std::shared_ptr<Physics::IPhysicsBody> other) {}

	void DamageBlock::handleCollision(std::shared_ptr<Physics::IPhysicsBody> other) {
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
}
