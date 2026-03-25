#include "Domain/Stage/OneWayPlatformStage.h"

namespace Jam::Domain::Stage {
    OneWayPlatformStage::OneWayPlatformStage(const StageObject& obj)
        : m_rect(obj.rect)
    {}
    
    void OneWayPlatformStage::update(double deltaTime) {
        // すり抜け床は位置が固定
    }
    
    RectF OneWayPlatformStage::getRenderRect() const {
        return m_rect;
    }
    
    StageType OneWayPlatformStage::getType() const {
        return StageType::OneWayPlatform;
    }
    
    Vec2 OneWayPlatformStage::getCurrentCenter() const {
        return m_rect.center();
    }
    
    /**
     * 床の上端のY座標を取得
     */
    double OneWayPlatformStage::getTopY() const {
        return m_rect.y;
    }
}