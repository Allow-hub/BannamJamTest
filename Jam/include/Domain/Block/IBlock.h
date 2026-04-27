#pragma once
#include "Domain/Block/BlockTypes.h"

namespace Jam::Domain::Block {
    /**
     * ブロックインターフェース
     * 1つのブロックオブジェクト（床・壁・動く床など）を表す
     */
    class IBlock {
    public:
        virtual ~IBlock() = default;

        // 更新処理（動くブロックで使用）
        virtual void update(double deltaTime) = 0;

        // 描画情報の取得
        virtual RectF getRenderRect() const = 0;
        virtual BlockType getType() const = 0;

        // 現在の中心位置を取得（物理同期用）
        virtual Vec2 getCurrentCenter() const = 0;
    };
}