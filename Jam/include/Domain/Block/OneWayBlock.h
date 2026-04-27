#pragma once
#include "Domain/Block/IBlock.h"

namespace Jam::Domain::Block {
    /**
     * すり抜ける床
     * 下から上への移動時は通過可能、上から着地可能
     */
    class OneWayBlock : public IBlock {
    private:
        RectF m_rect;

    public:
        OneWayBlock(const BlockObject& obj);

        void update(double deltaTime) override;

        RectF getRenderRect() const override;

        BlockType getType() const override;

        Vec2 getCurrentCenter() const override;

        /**
         * 床の上端のY座標を取得
         */
        double getTopY() const;
    };
}
