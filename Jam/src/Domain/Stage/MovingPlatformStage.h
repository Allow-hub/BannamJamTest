#pragma once
#include "IStage.h"

namespace Jam::Domain::Stage {
    /**
     * 蜍輔￥蠎・
     * 讓ｪ遘ｻ蜍輔∫ｸｦ遘ｻ蜍輔∝・驕句虚縺ｮ3遞ｮ鬘槭・蜍穂ｽ懊ヱ繧ｿ繝ｼ繝ｳ繧呈戟縺､
     */
    class MovingPlatformStage : public IStage {
    private:
        RectF m_baseRect;              // 蝓ｺ貅也洸蠖｢
        Vec2 m_baseCenter;             // 蝓ｺ貅紋ｸｭ蠢・ｽ咲ｽｮ
        Vec2 m_currentOffset;          // 迴ｾ蝨ｨ縺ｮ繧ｪ繝輔そ繝・ヨ
        MovementType m_movementType;   // 遘ｻ蜍輔ち繧､繝・
        double m_movementSpeed;        // 遘ｻ蜍暮溷ｺｦ・医ヴ繧ｯ繧ｻ繝ｫ/遘抵ｼ・
        double m_movementDistance;     // 遘ｻ蜍戊ｷ晞屬縺ｾ縺溘・蜊雁ｾ・
        bool m_loopMovement;           // 繝ｫ繝ｼ繝励☆繧九°
        double m_elapsedTime;          // 邨碁℃譎る俣
        
    public:
        MovingPlatformStage(const StageObject& obj)
            : m_baseRect(obj.rect)
            , m_baseCenter(obj.rect.center())
            , m_currentOffset(0, 0)
            , m_movementType(obj.movementType)
            , m_movementSpeed(obj.movementSpeed)
            , m_movementDistance(obj.movementDistance)
            , m_loopMovement(obj.loopMovement)
            , m_elapsedTime(0.0)
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
            return StageType::MovingPlatform;
        }
        
        Vec2 getCurrentCenter() const override {
            return m_baseCenter + m_currentOffset;
        }
        
    private:
        // 讓ｪ遘ｻ蜍輔・譖ｴ譁ｰ
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
        
        // 邵ｦ遘ｻ蜍輔・譖ｴ譁ｰ
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
        
        // 蜀・°蜍輔・譖ｴ譁ｰ
        void updateCircular(double deltaTime) {
            double circumference = 2.0 * Math::Pi * m_movementDistance;
            double angle = (m_movementSpeed * m_elapsedTime / circumference) * 2.0 * Math::Pi;
            
            m_currentOffset.x = Math::Cos(angle) * m_movementDistance;
            m_currentOffset.y = Math::Sin(angle) * m_movementDistance;
        }
    };
}
