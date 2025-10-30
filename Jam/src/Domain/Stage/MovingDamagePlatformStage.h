#pragma once
#include "IStage.h"

namespace Jam::Domain::Stage {
    /**
     * 動いてダメージを与える床
     * 横移動、縦移動、円運動の3種類の動作パターンを持ち、触れるとダメージを受ける
     */
    class MovingDamagePlatformStage : public IStage {
    private:
        RectF m_baseRect;              // 基準矩形
        Vec2 m_baseCenter;             // 基準中心位置
        Vec2 m_currentOffset;          // 現在のオフセット
        MovementType m_movementType;   // 移動タイプ
        double m_movementSpeed;        // 移動速度(ピクセル/秒)
        double m_movementDistance;     // 移動距離または半径
        bool m_loopMovement;           // ループするか
        double m_elapsedTime;          // 経過時間
        double m_damageAmount;         // 与えるダメージ量
        
    public:
        MovingDamagePlatformStage(const StageObject& obj)
            : m_baseRect(obj.rect)
            , m_baseCenter(obj.rect.center())
            , m_currentOffset(0, 0)
            , m_movementType(obj.movementType)
            , m_movementSpeed(obj.movementSpeed)
            , m_movementDistance(obj.movementDistance)
            , m_loopMovement(obj.loopMovement)
            , m_elapsedTime(0.0)
            , m_damageAmount(obj.damageAmount)
        {}
        
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
            return RectF(m_baseRect.pos + m_currentOffset, m_baseRect.size);
        }
        
        StageType getType() const override {
            return StageType::MovingDamagePlatform;
        }
        
        Vec2 getCurrentCenter() const override {
            return m_baseCenter + m_currentOffset;
        }
        
        /**
         * ダメージ量を取得
         */
        double getDamageAmount() const {
            return m_damageAmount;
        }
        
    private:
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