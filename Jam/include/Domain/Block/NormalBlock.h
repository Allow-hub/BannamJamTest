#pragma once
#include "Domain/Block/IBlock.h"

namespace Jam::Domain::Block {
    /**
     * 通常の静的ブロック
     * 壁・床などの動かないブロックオブジェクト
     */
    class NormalBlock : public IBlock {
    private:
        RectF m_rect;
        BlockType m_type;
        String m_texturePath;

    public:
        NormalBlock(const BlockObject& obj);

        // 静的なので何もしない
        void update(double deltaTime) override;

        RectF getRenderRect() const override;

        BlockType getType() const override;

        Vec2 getCurrentCenter() const override;

        // NormalBlock固有のメソッド: テクスチャパスを取得
        String getTexturePath() const;
    };
}
