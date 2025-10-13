#pragma once
#include "../Domain/Stage/IStagePhysicsManager.h"
#include "Siv3DPhysicsBody.h"

namespace Jam::Infrastructure {
    
    class StagePhysicsManager : public Domain::Stage::IStagePhysicsManager {
    private:
        P2World& m_world;
        Array<std::shared_ptr<Physics::Siv3DPhysicsBody>> m_physicsBodies;
        
    public:
        explicit StagePhysicsManager(P2World& world);
        
        void createPhysicsBodies(const Array<Domain::Stage::StageObject>& objects) override;
        void clearPhysicsBodies() override;
        const Array<std::shared_ptr<Domain::Physics::IPhysicsBody>>& getPhysicsBodies() const override;
        
    private:
        P2BodyType getBodyType(Domain::Stage::CollisionType type) const;
    };
}