#pragma once
#include "IStage.h"
#include "../Physics/ICollisionListener.h"
#include "../Physics/IPhysicsBody.h"
#include "../Events/GameEvents.h"
#include <memory>

namespace Jam::Domain::Stage {
    /**
     * 動いてダメージを与える床
     * 横移動、縦移動、円運動の3種類の動作パターンを持ち、触れるとダメージを受ける
     * ICollisionListenerを継承し、物理エンジンの衝突検知を利用
     */
    class MovingDamagePlatformStage : public IStage
                                    , public Physics::ICollisionListener {
    private:
        RectF m_baseRect;              // 基準矩形
        Vec2 m_currentOffset;          // 現在のオフセット
        MovementType m_movementType;   // 移動タイプ
        double m_movementSpeed;        // 移動速度(ピクセル/秒)
        double m_movementDistance;     // 移動距離または半径
        bool m_loopMovement;           // ループするか
        double m_elapsedTime;          // 経過時間
        double m_damageAmount;         // 与えるダメージ量
        
        std::shared_ptr<Physics::IPhysicsBody> m_body;  // メイン物理ボディ
        Events::GameEventQueue& m_eventQueue;  // イベントキュー
        Physics::PhysicsBodyID m_playerId;  // プレイヤーID
        
        // 自身へのshared_ptr（ライフタイム管理用）
        std::shared_ptr<MovingDamagePlatformStage> m_selfPtr;
        
        // ダメージ間隔管理
        double m_damageInterval = 0.5;  // ダメージを与える間隔（秒）
        double m_lastDamageTime = -999.0;  // 最後にダメージを与えた時刻
        
    public:
        MovingDamagePlatformStage(const StageObject& obj,
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
            auto noOpDeleter = [](MovingDamagePlatformStage*) {
                // 何もしない: unique_ptrがオブジェクトの破棄を担当する
            };
            m_selfPtr = std::shared_ptr<MovingDamagePlatformStage>(this, noOpDeleter);
            
            // ICollisionListenerとしてキャスト
            auto listener = std::dynamic_pointer_cast<Physics::ICollisionListener>(m_selfPtr);
            
            // 物理ボディに衝突リスナーを設定
            m_body->setCollisionListener(listener);
        }
        
        void update(double deltaTime) override {
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
        
        RectF getRenderRect() const override {
            Vec2 currentCenter = m_baseRect.center() + m_currentOffset;
            Vec2 topLeft = currentCenter - m_baseRect.size / 2.0;
            return RectF(topLeft, m_baseRect.size);
        }
        
        StageType getType() const override {
            return StageType::MovingDamagePlatform;
        }
        
        Vec2 getCurrentCenter() const override {
            return m_baseRect.center() + m_currentOffset;
        }
        
        // ICollisionListener実装
        void onCollisionEnter(std::shared_ptr<Physics::IPhysicsBody> other) override {
            handleCollision(other);
        }
        
        void onCollisionStay(std::shared_ptr<Physics::IPhysicsBody> other) override {
            handleCollision(other);
        }
        
        void onCollisionExit(std::shared_ptr<Physics::IPhysicsBody> other) override {
            // 衝突終了時は何もしない
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
        
        // 横移動の更新
        void updateHorizontal(double deltaTime) {
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
        void updateVertical(double deltaTime) {
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
        void updateCircular(double deltaTime) {
            double circumference = 2.0 * Math::Pi * m_movementDistance;
            double angle = (m_movementSpeed * m_elapsedTime / circumference) * 2.0 * Math::Pi;
            
            m_currentOffset.x = Math::Cos(angle) * m_movementDistance;
            m_currentOffset.y = Math::Sin(angle) * m_movementDistance;
        }
    };
}