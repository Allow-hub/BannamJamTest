#pragma once
#include "StageTypes.h"
#include "../Physics/IPhysicsBody.h"

namespace Jam::Domain::Stage {
    
    // ステージオブジェクトを物理世界に登録するためのインターフェース
    class IStagePhysicsManager {
    public:
        virtual ~IStagePhysicsManager() = default;
        
        // ステージオブジェクトを物理世界に登録
        virtual void createPhysicsBodies(const Array<StageObject>& objects) = 0;
        
        // 物理ボディをクリア
        virtual void clearPhysicsBodies() = 0;
        
        // 物理ボディの取得（当たり判定チェック用）
        virtual const Array<std::shared_ptr<Physics::IPhysicsBody>>& getPhysicsBodies() const = 0;
    };
}