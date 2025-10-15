#pragma once
#include "StageTypes.h"
#include "../Physics/IPhysicsBody.h"

namespace Jam::Domain::Stage {
    class IStage {
    public:
        virtual ~IStage() = default;
        
        // 基本機能
        virtual Array<StageObject> getRenderableObjects() const = 0;
        virtual Array<std::shared_ptr<Physics::IPhysicsBody>> getPhysicsBodies() const = 0;
        
        // オブジェクト管理
        virtual bool destroyObject(const String& objectId) = 0;
        virtual bool isObjectDestroyed(const String& objectId) const = 0;
        
        // 動的機能（静的ステージでは空実装）
        virtual void update(double deltaTime) {}
        virtual void setMovementSpeed(const String& objectId, Vec2 speed) {}
        virtual void setMovementPath(const String& objectId, Array<Vec2> path) {}
    };
}