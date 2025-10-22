#include "StageFactory.h"

namespace Jam::Infrastructure {
    
    StageCreationResult StageFactory::createStagesFromFile(
        const String& filename,
        std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory
    ) {
        StageCreationResult result;
        
        Array<Domain::Stage::StageObject> objects;
        if (!Stage::StageLoader::loadStageFromFile(filename, objects)) {
            return result;
        }
        
        for (const auto& obj : objects) {
            std::shared_ptr<Domain::Physics::IPhysicsBody> body;
            auto stage = createStage(obj, bodyFactory, body);
            if (stage && body) {
                result.stages.push_back(std::move(stage));
                result.physicsBodies.push_back(body);
            }
        }
        
        return result;
    }
    
    std::unique_ptr<Domain::Stage::IStage> StageFactory::createStage(
        const Domain::Stage::StageObject& obj,
        std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory,
        std::shared_ptr<Domain::Physics::IPhysicsBody>& outBody
    ) {
        if (!bodyFactory) {
            return nullptr;
        }
        
        auto physicsLayer = getPhysicsLayerFromType(obj.type);
        
        outBody = bodyFactory->createBody(
            obj.rect.center(),
            obj.rect.size,
            P2BodyType::Static,
            Domain::Physics::PhysicsMaterial{ 1.0, 0.0, 0.0 }
        );
        
        if (!outBody) {
            return nullptr;
        }
        
        outBody->setLayer(physicsLayer);
        
        switch (obj.type) {
            case Domain::Stage::StageType::MovingPlatform:
                return std::make_unique<Domain::Stage::MovingPlatformStage>(obj);
                
            case Domain::Stage::StageType::Normal:
            case Domain::Stage::StageType::Platform:
            case Domain::Stage::StageType::Hazard:
            case Domain::Stage::StageType::Trigger:
            case Domain::Stage::StageType::Breakable:
            default:
                return std::make_unique<Domain::Stage::NormalStage>(obj);
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
