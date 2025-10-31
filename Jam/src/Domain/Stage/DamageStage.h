#pragma once
#include "IStage.h"
#include "../Physics/ICollisionListener.h"
#include "../Physics/IPhysicsBody.h"
#include "../Events/GameEvents.h"
#include "../ITakeDamageable.h"
#include <memory>

namespace Jam::Domain::Stage {
    /**
     * ダメージを与える床
     * プレイヤーが触れるとダメージを受ける静的なステージオブジェクト
     * ICollisionListenerを継承し、物理エンジンの衝突検知を利用
     */
    class DamageStage : public IStage
                      , public Physics::ICollisionListener {
    private:
        RectF m_rect;
        double m_damageAmount;  // 与えるダメージ量
        std::shared_ptr<Physics::IPhysicsBody> m_body;  // メイン物理ボディ
        std::vector<std::shared_ptr<Physics::IPhysicsBody>> m_additionalBodies;  // 追加の物理ボディ
        Events::GameEventQueue& m_eventQueue;  // イベントキュー
        Physics::PhysicsBodyID m_playerId;  // プレイヤーID
        
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
        
        // 初期化メソッド（shared_ptr管理下になった後に呼ぶ）
        void init() {
            if (m_body) {
                // thisポインタを直接渡す（ICollisionListenerとして）
                m_body->setCollisionListener(std::shared_ptr<Physics::ICollisionListener>(
                    std::shared_ptr<void>(), this));
            }
        }
        
        // 追加の物理ボディを登録（groundSide展開時に使用）
        void addAdditionalBody(std::shared_ptr<Physics::IPhysicsBody> body) {
            if (body) {
                m_additionalBodies.push_back(body);
                body->setCollisionListener(std::shared_ptr<Physics::ICollisionListener>(
                    std::shared_ptr<void>(), this));
            }
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
        
        double getDamageAmount() const {
            return m_damageAmount;
        }
        
        // ICollisionListener実装
        void onCollisionEnter(std::shared_ptr<Physics::IPhysicsBody> other) override {
            handleCollision(other);
        }
        
        void onCollisionStay(std::shared_ptr<Physics::IPhysicsBody> other) override {
            handleCollision(other);
        }
        
        void onCollisionExit(std::shared_ptr<Physics::IPhysicsBody> other) override {
            // 何もしない
        }
        
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
