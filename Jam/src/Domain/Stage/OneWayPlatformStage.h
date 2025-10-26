#pragma once
#include "IStage.h"

namespace Jam::Domain::Stage {
    /**
     * すり抜ける床
     * 下から上への移動時は通過可能、上から着地可能
     */
    class OneWayPlatformStage : public IStage {
    private:
        RectF m_rect;
        
    public:
        OneWayPlatformStage(const StageObject& obj)
            : m_rect(obj.rect)
        {}
        
        void update(double deltaTime) override {
            // すり抜け床は位置が固定
        }
        
        RectF getRenderRect() const override {
            return m_rect;
        }
        
        StageType getType() const override {
            return StageType::OneWayPlatform;
        }
        
        Vec2 getCurrentCenter() const override {
            return m_rect.center();
        }
        
        /**
         * 床の上面のY座標を取得
         */
        double getTopY() const {
            return m_rect.y;
        }
    };
}
