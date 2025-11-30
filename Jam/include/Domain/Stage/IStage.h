#pragma once
#include "Domain/Stage/StageTypes.h"

namespace Jam::Domain::Stage {
    /**
     * ステージインターフェース
     * 1つのステージオブジェクト（床・壁・動く床など）を表す
     */
    class IStage {
    public:
        virtual ~IStage() = default;
        
        // 更新処理（動くステージで使用）
        virtual void update(double deltaTime) = 0;
        
        // 描画情報の取得
        virtual RectF getRenderRect() const = 0;
        virtual StageType getType() const = 0;
        
        // 現在の中心位置を取得（物理同期用）
        virtual Vec2 getCurrentCenter() const = 0;
    };
}