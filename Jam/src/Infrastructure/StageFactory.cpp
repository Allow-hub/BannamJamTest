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
            auto createdObjects = expandObjectByGroundSide(obj);
            
            Array<size_t> bodyIndicesForThisStage;
            Vec2 baseCenter = obj.rect.center();
            
            if (!createdObjects.isEmpty()) {
                std::shared_ptr<Domain::Physics::IPhysicsBody> body;
                auto stage = createStage(createdObjects[0], bodyFactory, body);
                
                if (stage && body) {
                    size_t bodyIndex = result.physicsBodies.size();
                    result.physicsBodies.push_back(body);
                    bodyIndicesForThisStage.push_back(bodyIndex);
                    
                    Vec2 offset = createdObjects[0].rect.center() - baseCenter;
                    result.bodyOffsets.push_back(offset);
                    
                    for (size_t i = 1; i < createdObjects.size(); ++i) {
                        std::shared_ptr<Domain::Physics::IPhysicsBody> additionalBody;
                        createStage(createdObjects[i], bodyFactory, additionalBody);
                        if (additionalBody) {
                            size_t additionalBodyIndex = result.physicsBodies.size();
                            result.physicsBodies.push_back(additionalBody);
                            bodyIndicesForThisStage.push_back(additionalBodyIndex);
                            
                            Vec2 additionalOffset = createdObjects[i].rect.center() - baseCenter;
                            result.bodyOffsets.push_back(additionalOffset);
                        }
                    }
                    
                    result.stages.push_back(std::move(stage));
                    result.bodyIndices.push_back(bodyIndicesForThisStage);
                }
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
        
        auto physicsLayer = getPhysicsLayerFromType(obj.type, obj.groundSide);
        
        P2BodyType bodyType = (obj.type == Domain::Stage::StageType::MovingPlatform)
            ? P2BodyType::Kinematic
            : P2BodyType::Static;
        
        outBody = bodyFactory->createBody(
            obj.rect.center(),
            obj.rect.size,
            bodyType,
            Domain::Physics::PhysicsMaterial{ 1.0, 0.0, 0.0 }
        );
        
        if (!outBody) {
            return nullptr;
        }
        
        outBody->setLayer(physicsLayer);
        
        if (obj.type == Domain::Stage::StageType::OneWayPlatform) {
            outBody->setFilter(Jam::Infrastructure::PhysicsFilter::OneWayPlatform);
        }
        
        switch (obj.type) {
            case Domain::Stage::StageType::MovingPlatform:
                return std::make_unique<Domain::Stage::MovingPlatformStage>(obj);
                
            case Domain::Stage::StageType::OneWayPlatform:
                return std::make_unique<Domain::Stage::OneWayPlatformStage>(obj);
                
            case Domain::Stage::StageType::Normal:
            default:
                return std::make_unique<Domain::Stage::NormalStage>(obj);
        }
    }
    
    Domain::Physics::PhysicsLayer StageFactory::getPhysicsLayerFromType(
        Domain::Stage::StageType type, 
        Domain::Stage::GroundSide groundSide
    ) {
        using namespace Domain::Physics;
        
        if (groundSide == Domain::Stage::GroundSide::None) {
            return PhysicsLayer::Wall;
        }
        
        switch (type) {
            case Domain::Stage::StageType::Normal:
            case Domain::Stage::StageType::MovingPlatform:
                return PhysicsLayer::Ground;
                
            case Domain::Stage::StageType::OneWayPlatform:
                return PhysicsLayer::OneWayPlatform;
                
            default:
                return PhysicsLayer::Ground;
        }
    }

    Array<Domain::Stage::StageObject> StageFactory::expandObjectByGroundSide(const Domain::Stage::StageObject& obj) {
        using namespace Domain::Stage;
        Array<StageObject> result;
        
        constexpr double groundThickness = 5.0;
        
        if (obj.groundSide == GroundSide::All) {
            result.push_back(obj);
            return result;
        }
        
        if (obj.groundSide != GroundSide::None) {
            StageObject wallObj = obj;
            wallObj.groundSide = GroundSide::None;
            result.push_back(wallObj);
        }
        
        StageObject groundObj = obj;
        groundObj.groundSide = GroundSide::Up;
        
        switch (obj.groundSide) {
            case GroundSide::Up:
                groundObj.rect = RectF(
                    obj.rect.x,
                    obj.rect.y,
                    obj.rect.w,
                    groundThickness
                );
                result.push_back(groundObj);
                break;
                
            case GroundSide::None:
                if (result.empty()) {
                    result.push_back(obj);
                }
                break;
                
            default:
                result.push_back(obj);
                break;
        }
        
        return result;
    }
}
