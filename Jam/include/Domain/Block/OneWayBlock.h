#pragma once
#include "Domain/Stage/IStage.h"

namespace Jam::Domain::Stage {
    /**
     * すり抜ける床
     * 下から上への移動時は通過可能、上から着地可能
     */
    class OneWayPlatformStage : public IStage {
    private:
        RectF m_rect;
        
    public:
        OneWayPlatformStage(const StageObject& obj);
        
        void update(double deltaTime) override;
        
        RectF getRenderRect() const override;
        
        StageType getType() const override;
        
        Vec2 getCurrentCenter() const override;
        
        /**
         * 床の上端のY座標を取得
         */
        double getTopY() const;
    };
}
