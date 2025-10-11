#pragma once
#include <memory>
#include "IPhysicsBody.h"

namespace Jam::Domain::Physics
{
    class IPhysicsWorld
    {
    public:
        virtual ~IPhysicsWorld() = default;

        // 物理ボディを生成する
        virtual std::shared_ptr<IPhysicsBody> createBody(const Vec2& position, const SizeF& size) = 0;

        // 世界を一定時間進める
        virtual void step(double deltaTime) = 0;
    };
}
