#pragma once
#include "IStage.h"

namespace Jam::Domain::Stage {
    /**
     * 通常の静的ステージ
     * 壁・床などの動かないステージオブジェクト
     */
    class NormalStage : public IStage {
    private:
        RectF m_rect;
        StageType m_type;
        
    public:
        NormalStage(const StageObject& obj)
            : m_rect(obj.rect)
            , m_type(obj.type)
        {}
        
        // 静的なので何もしない
        void update(double deltaTime) override {}
        
        RectF getRenderRect() const override {
            return m_rect;
        }
        
        StageType getType() const override {
            return m_type;
        }
        
        Vec2 getCurrentCenter() const override {
            return m_rect.center();
        }
    };
}
