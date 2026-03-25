#include "Domain/Stage/MovingPlatformStage.h"

namespace Jam::Domain::Stage {
    MovingPlatformStage::MovingPlatformStage(const StageObject& obj)
        : m_baseRect(obj.rect)
        , m_baseCenter(obj.rect.center())
        , m_currentOffset(0, 0)
        , m_movementType(obj.movementType)
        , m_movementSpeed(obj.movementSpeed)
        , m_movementDistance(obj.movementDistance)
        , m_loopMovement(obj.loopMovement)
        , m_elapsedTime(0.0)
    {}
    
    void MovingPlatformStage::update(double deltaTime) {
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
    
    RectF MovingPlatformStage::getRenderRect() const {
        Vec2 currentCenter = m_baseCenter + m_currentOffset;
        Vec2 topLeft = currentCenter - m_baseRect.size / 2.0;
        return RectF(topLeft, m_baseRect.size);
    }
    
    StageType MovingPlatformStage::getType() const {
        return StageType::MovingPlatform;
    }
    
    Vec2 MovingPlatformStage::getCurrentCenter() const {
        return m_baseCenter + m_currentOffset;
    }
    
    // 横移動の更新
    void MovingPlatformStage::updateHorizontal(double deltaTime) {
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
    void MovingPlatformStage::updateVertical(double deltaTime) {
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
    void MovingPlatformStage::updateCircular(double deltaTime) {
        double circumference = 2.0 * Math::Pi * m_movementDistance;
        double angle = (m_movementSpeed * m_elapsedTime / circumference) * 2.0 * Math::Pi;
        
        m_currentOffset.x = Math::Cos(angle) * m_movementDistance;
        m_currentOffset.y = Math::Sin(angle) * m_movementDistance;
    }
}