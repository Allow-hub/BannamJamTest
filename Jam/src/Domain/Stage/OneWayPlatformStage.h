#pragma once
#include "IStage.h"

namespace Jam::Domain::Stage {
    /**
     * 縺吶ｊ謚懊￠繧句ｺ・
     * 荳九°繧我ｸ翫∈縺ｮ遘ｻ蜍墓凾縺ｯ騾夐℃蜿ｯ閭ｽ縲∽ｸ翫°繧臥捩蝨ｰ蜿ｯ閭ｽ
     */
    class OneWayPlatformStage : public IStage {
    private:
        RectF m_rect;
        
    public:
        OneWayPlatformStage(const StageObject& obj)
            : m_rect(obj.rect)
        {}
        
        void update(double deltaTime) override {
            // 縺吶ｊ謚懊￠蠎翫・菴咲ｽｮ縺悟崋螳・
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
         * 蠎翫・荳企擇縺ｮY蠎ｧ讓吶ｒ蜿門ｾ・
         */
        double getTopY() const {
            return m_rect.y;
        }
    };
}
