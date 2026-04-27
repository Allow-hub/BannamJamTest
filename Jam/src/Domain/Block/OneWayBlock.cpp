#include "Domain/Block/OneWayBlock.h"

namespace Jam::Domain::Block {
    OneWayBlock::OneWayBlock(const BlockObject& obj)
        : m_rect(obj.rect)
    {}
    
    void OneWayBlock::update(double deltaTime) {
        // すり抜け床は位置が固定
    }

    RectF OneWayBlock::getRenderRect() const {
        return m_rect;
    }
    
    BlockType OneWayBlock::getType() const {
        return BlockType::OneWayPlatform;
    }
    
    Vec2 OneWayBlock::getCurrentCenter() const {
        return m_rect.center();
    }
    
    /**
     * 床の上端のY座標を取得
     */
    double OneWayBlock::getTopY() const {
        return m_rect.y;
    }
}
