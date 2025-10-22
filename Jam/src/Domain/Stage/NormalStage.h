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
        std::shared_ptr<Physics::IPhysicsBody> m_body;
        
    public:
        NormalStage(const StageObject& obj, std::shared_ptr<Physics::IPhysicsBody> body)
            : m_rect(obj.rect)
            , m_type(obj.type)
            , m_body(body)
        {}
        
        // 静的なので何もしない
        void update(double deltaTime) override {}
        
        RectF getRenderRect() const override {
            return m_rect;
        }
        
        StageType getType() const override {
            return m_type;
        }
        
        std::shared_ptr<Physics::IPhysicsBody> getPhysicsBody() const override {
            return m_body;
        }
    };
}
