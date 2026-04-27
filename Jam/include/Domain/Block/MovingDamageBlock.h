#pragma once
#include "Domain/Block/IBlock.h"
#include "Domain/Physics/ICollisionListener.h"
#include "Domain/Physics/IPhysicsBody.h"
#include "Domain/Events/GameEvents.h"
#include <memory>

namespace Jam::Domain::Block {
    /**
     * 動いてダメージを与える床
     * 横移動、縦移動、円運動の3種類の動作パターンを持ち、触れるとダメージを受ける
     * ICollisionListenerを継承し、物理エンジンの衝突検知を利用
     */
    class MovingDamageBlock : public IBlock
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
        std::shared_ptr<MovingDamageBlock> m_selfPtr;

        // ダメージ間隔管理
        double m_damageInterval = 0.5;  // ダメージを与える間隔（秒）
        double m_lastDamageTime = -999.0;  // 最後にダメージを与えた時刻

    public:
        MovingDamageBlock(const BlockObject& obj,
                                 std::shared_ptr<Physics::IPhysicsBody> body,
                                 Events::GameEventQueue& eventQueue,
                                 Physics::PhysicsBodyID playerId);

        /**
         * 初期化メソッド
         * コンストラクタ後、ブロックがunique_ptrで管理された後に呼び出す必要がある
         */
        void init();

        void update(double deltaTime) override;

        RectF getRenderRect() const override;

        BlockType getType() const override;

        Vec2 getCurrentCenter() const override;

        // ICollisionListener実装
        void onCollisionEnter(std::shared_ptr<Physics::IPhysicsBody> other) override;

        void onCollisionStay(std::shared_ptr<Physics::IPhysicsBody> other) override;

        void onCollisionExit(std::shared_ptr<Physics::IPhysicsBody> other) override;

    private:
        void handleCollision(std::shared_ptr<Physics::IPhysicsBody> other);

        // 横移動の更新
        void updateHorizontal(double deltaTime);

        // 縦移動の更新
        void updateVertical(double deltaTime);

        // 円運動の更新
        void updateCircular(double deltaTime);
    };
}