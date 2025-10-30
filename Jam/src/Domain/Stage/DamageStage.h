#pragma once
#include "IStage.h"

namespace Jam::Domain::Stage {
    /**
     * ダメージを与える床
     * プレイヤーが触れるとダメージを受ける静的なステージオブジェクト
     */
    class DamageStage : public IStage {
    private:
        RectF m_rect;
        double m_damageAmount;  // 与えるダメージ量
        
    public:
        DamageStage(const StageObject& obj)
            : m_rect(obj.rect)
            , m_damageAmount(obj.damageAmount)
        {}
        
        // 静的なので何もしない
        void update(double deltaTime) override {}
        
        RectF getRenderRect() const override {
            return m_rect;
        }
        
        StageType getType() const override {
            return StageType::DamagePlatform;
        }
        
        Vec2 getCurrentCenter() const override {
            return m_rect.center();
        }
        
        /**
         * ダメージ量を取得
         */
        double getDamageAmount() const {
            return m_damageAmount;
        }
    };
}
