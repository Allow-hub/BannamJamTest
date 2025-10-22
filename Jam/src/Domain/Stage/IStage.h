#pragma once
#include "StageTypes.h"
#include "../Physics/IPhysicsBody.h"

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
        
        // 物理ボディの取得
        virtual std::shared_ptr<Physics::IPhysicsBody> getPhysicsBody() const = 0;
    };
}