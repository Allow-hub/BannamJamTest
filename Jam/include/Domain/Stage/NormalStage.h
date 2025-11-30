#pragma once
#include "Domain/Stage/IStage.h"

namespace Jam::Domain::Stage {
    /**
     * 通常の静的ステージ
     * 壁・床などの動かないステージオブジェクト
     */
    class NormalStage : public IStage {
    private:
        RectF m_rect;
        StageType m_type;
        String m_texturePath;
        
    public:
        NormalStage(const StageObject& obj);
        
        // 静的なので何もしない
        void update(double deltaTime) override;
        
        RectF getRenderRect() const override;
        
        StageType getType() const override;
        
        Vec2 getCurrentCenter() const override;
        
        // NormalStage固有のメソッド: テクスチャパスを取得
        String getTexturePath() const;
    };
}
