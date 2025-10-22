#pragma once
#include "IStage.h"

namespace Jam::Domain::Stage {
    /**
     * 動く床
     * 直線移動や円運動などの移動ロジックを持つステージオブジェクト
     */
    class MovingPlatformStage : public IStage {
    private:
        RectF m_baseRect;           // 基準矩形
        Vec2 m_currentOffset;       // 現在のオフセット
        Vec2 m_movementSpeed;       // 移動速度
        Array<Vec2> m_movementPath; // 移動パス
        bool m_loopMovement;        // ループするか
        size_t m_currentPathIndex;  // 現在のパスインデックス
        double m_elapsedTime;       // 経過時間
        std::shared_ptr<Physics::IPhysicsBody> m_body;
        
    public:
        MovingPlatformStage(
            const StageObject& obj,
            std::shared_ptr<Physics::IPhysicsBody> body
        )
            : m_baseRect(obj.rect)
            , m_currentOffset(0, 0)
            , m_movementSpeed(obj.movementSpeed)
            , m_movementPath(obj.movementPath)
            , m_loopMovement(obj.loopMovement)
            , m_currentPathIndex(0)
            , m_elapsedTime(0.0)
            , m_body(body)
        {}
        
        void update(double deltaTime) override {
            m_elapsedTime += deltaTime;
            
            if (m_movementSpeed != Vec2(0, 0)) {
                m_currentOffset += m_movementSpeed * deltaTime;
                
                if (m_body) {
                    Vec2 newPos = m_baseRect.center() + m_currentOffset;
                    m_body->setPos(newPos);
                }
            }
        }
        
        RectF getRenderRect() const override {
            return RectF(m_baseRect.pos + m_currentOffset, m_baseRect.size);
        }
        
        StageType getType() const override {
            return StageType::MovingPlatform;
        }
        
        std::shared_ptr<Physics::IPhysicsBody> getPhysicsBody() const override {
            return m_body;
        }
    };
}
