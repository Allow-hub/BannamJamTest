#include "Domain/Block/MovingDamageBlock.h"

namespace Jam::Domain::Block {
    MovingDamageBlock::MovingDamageBlock(const BlockObject& obj,
                             std::shared_ptr<Physics::IPhysicsBody> body,
                             Events::GameEventQueue& eventQueue,
                             Physics::PhysicsBodyID playerId)
        : m_baseRect(obj.rect)
        , m_currentOffset(0, 0)
        , m_movementType(obj.movementType)
        , m_movementSpeed(obj.movementSpeed)
        , m_movementDistance(obj.movementDistance)
        , m_loopMovement(obj.loopMovement)
        , m_elapsedTime(0.0)
        , m_damageAmount(obj.damageAmount)
        , m_body(body)
        , m_eventQueue(eventQueue)
        , m_playerId(playerId)
    {
    }
    
    void MovingDamageBlock::init() {
        if (!m_body) {
            return;
        }
        
        // 自身へのshared_ptrを作成（ライフタイム管理はunique_ptrが行う）
        // カスタムデリーター: 削除時に何もしない（unique_ptrが破棄を担当）
        auto noOpDeleter = [](MovingDamageBlock*) {
            // 何もしない: unique_ptrがオブジェクトの破棄を担当する
        };
        m_selfPtr = std::shared_ptr<MovingDamageBlock>(this, noOpDeleter);
        
        // ICollisionListenerとしてキャスト
        auto listener = std::dynamic_pointer_cast<Physics::ICollisionListener>(m_selfPtr);
        
        // 物理ボディに衝突リスナーを設定
        m_body->setCollisionListener(listener);
    }
    
    void MovingDamageBlock::update(double deltaTime) {
        m_elapsedTime += deltaTime;
        
        switch (m_movementType) {
            case MovementType::Horizontal:
                updateHorizontal(deltaTime);
                break;
            case MovementType::Vertical:
                updateVertical(deltaTime);
                break;
            case MovementType::Circular:
                updateCircular(deltaTime);
                break;
        }
    }
    
    RectF MovingDamageBlock::getRenderRect() const {
        Vec2 currentCenter = m_baseRect.center() + m_currentOffset;
        Vec2 topLeft = currentCenter - m_baseRect.size / 2.0;
        return RectF(topLeft, m_baseRect.size);
    }
    
    BlockType MovingDamageBlock::getType() const {
        return BlockType::MovingDamagePlatform;
    }
    
    Vec2 MovingDamageBlock::getCurrentCenter() const {
        return m_baseRect.center() + m_currentOffset;
    }
    
    // ICollisionListener実装
    void MovingDamageBlock::onCollisionEnter(std::shared_ptr<Physics::IPhysicsBody> other) {
        handleCollision(other);
    }
    
    void MovingDamageBlock::onCollisionStay(std::shared_ptr<Physics::IPhysicsBody> other) {
        handleCollision(other);
    }
    
    void MovingDamageBlock::onCollisionExit(std::shared_ptr<Physics::IPhysicsBody> other) {
        // 衝突終了時は何もしない
    }
    
    void MovingDamageBlock::handleCollision(std::shared_ptr<Physics::IPhysicsBody> other) {
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
    
    // 横移動の更新
    void MovingDamageBlock::updateHorizontal(double deltaTime) {
        double progress = (m_movementSpeed * m_elapsedTime) / m_movementDistance;
        
        if (m_loopMovement) {
            progress = Math::Fmod(progress, 2.0);
            if (progress > 1.0) {
                progress = 2.0 - progress;
            }
        } else {
            progress = Math::Clamp(progress, 0.0, 1.0);
        }
        
        m_currentOffset.x = progress * m_movementDistance;
        m_currentOffset.y = 0;
    }
    
    // 縦移動の更新
    void MovingDamageBlock::updateVertical(double deltaTime) {
        double progress = (m_movementSpeed * m_elapsedTime) / m_movementDistance;
        
        if (m_loopMovement) {
            progress = Math::Fmod(progress, 2.0);
            if (progress > 1.0) {
                progress = 2.0 - progress;
            }
        } else {
            progress = Math::Clamp(progress, 0.0, 1.0);
        }
        
        m_currentOffset.x = 0;
        m_currentOffset.y = progress * m_movementDistance;
    }
    
    // 円運動の更新
    void MovingDamageBlock::updateCircular(double deltaTime) {
        double circumference = 2.0 * Math::Pi * m_movementDistance;
        double angle = (m_movementSpeed * m_elapsedTime / circumference) * 2.0 * Math::Pi;
        
        m_currentOffset.x = Math::Cos(angle) * m_movementDistance;
        m_currentOffset.y = Math::Sin(angle) * m_movementDistance;
    }
}