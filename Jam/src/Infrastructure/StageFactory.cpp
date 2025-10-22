#include "StageFactory.h"

namespace Jam::Infrastructure {
    
    Array<std::unique_ptr<Domain::Stage::IStage>> StageFactory::createStagesFromFile(
        const String& filename,
        std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory
    ) {
        Array<std::unique_ptr<Domain::Stage::IStage>> stages;
        
        Array<Domain::Stage::StageObject> objects;
        if (!Stage::StageLoader::loadStageFromFile(filename, objects)) {
            return stages;
        }
        
        for (const auto& obj : objects) {
            auto stage = createStage(obj, bodyFactory);
            if (stage) {
                stages.push_back(std::move(stage));
            }
        }
        
        return stages;
    }
    
    std::unique_ptr<Domain::Stage::IStage> StageFactory::createStage(
        const Domain::Stage::StageObject& obj,
        std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory
    ) {
        if (!bodyFactory) {
            return nullptr;
        }
        
        auto physicsLayer = getPhysicsLayerFromType(obj.type);
        
        auto body = bodyFactory->createBody(
            obj.rect.center(),
            obj.rect.size,
            P2BodyType::Static,
            Domain::Physics::PhysicsMaterial{ 1.0, 0.0, 0.0 }
        );
        
        if (!body) {
            return nullptr;
        }
        
        body->setLayer(physicsLayer);
        
        switch (obj.type) {
            case Domain::Stage::StageType::MovingPlatform:
                return std::make_unique<Domain::Stage::MovingPlatformStage>(obj, body);
                
            case Domain::Stage::StageType::Normal:
            case Domain::Stage::StageType::Platform:
            case Domain::Stage::StageType::Hazard:
            case Domain::Stage::StageType::Trigger:
            case Domain::Stage::StageType::Breakable:
            default:
                return std::make_unique<Domain::Stage::NormalStage>(obj, body);
        }
    }
    
    Domain::Physics::PhysicsLayer StageFactory::getPhysicsLayerFromType(Domain::Stage::StageType type) {
        using namespace Domain::Physics;
        
        switch (type) {
            case Domain::Stage::StageType::Normal:
            case Domain::Stage::StageType::Platform:
            case Domain::Stage::StageType::MovingPlatform:
                return PhysicsLayer::Ground;
                
            case Domain::Stage::StageType::Hazard:
                return PhysicsLayer::Enemy;
                
            case Domain::Stage::StageType::Trigger:
            case Domain::Stage::StageType::Breakable:
            default:
                return PhysicsLayer::Ground;
        }
    }
}
