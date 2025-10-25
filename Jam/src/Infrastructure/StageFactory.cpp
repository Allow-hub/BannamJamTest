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
            // groundSideに基づいて複数のオブジェクトを生成
            auto createdObjects = expandObjectByGroundSide(obj);
            
            for (const auto& expandedObj : createdObjects) {
                std::shared_ptr<Domain::Physics::IPhysicsBody> body;
                auto stage = createStage(expandedObj, bodyFactory, body);
                if (stage && body) {
                    result.stages.push_back(std::move(stage));
                    result.physicsBodies.push_back(body);
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
            case Domain::Stage::StageType::MovingPlatform:
                return PhysicsLayer::Ground;
                
            case Domain::Stage::StageType::Wall:
                return PhysicsLayer::Wall;
                
            case Domain::Stage::StageType::Hazard:
                return PhysicsLayer::Enemy;
                
            case Domain::Stage::StageType::Trigger:
            case Domain::Stage::StageType::Breakable:
            default:
                return PhysicsLayer::Ground;
        }
    }

    Array<Domain::Stage::StageObject> StageFactory::expandObjectByGroundSide(const Domain::Stage::StageObject& obj) {
        using namespace Domain::Stage;
        Array<StageObject> result;
        
        const double groundThickness = 5.0;  // 地上判定の厚さ
        
        // groundSideが"All"の場合は、そのまま返す（従来の挙動）
        if (obj.groundSide == GroundSide::All) {
            result.push_back(obj);
            return result;
        }
        
        // groundSideが"None"以外の場合、壁本体を追加
        if (obj.groundSide != GroundSide::None) {
            StageObject wallObj = obj;
            wallObj.type = StageType::Wall;  // 壁として扱う
            wallObj.groundSide = GroundSide::None;  // 再帰防止
            result.push_back(wallObj);
        }
        
        // 指定された面に薄い地上判定を追加
        StageObject groundObj = obj;
        groundObj.type = StageType::Normal;  // 床として扱う
        groundObj.groundSide = GroundSide::None;  // 再帰防止
        
        switch (obj.groundSide) {
            case GroundSide::Up:
                // 上面に薄い床を配置
                groundObj.rect = RectF(
                    obj.rect.x,
                    obj.rect.y,
                    obj.rect.w,
                    groundThickness
                );
                result.push_back(groundObj);
                break;
                
            case GroundSide::None:
                // 地上判定なし（壁のみ）
                if (result.empty()) {
                    StageObject wallOnlyObj = obj;
                    wallOnlyObj.type = StageType::Wall;
                    result.push_back(wallOnlyObj);
                }
                break;
                
            default:
                // デフォルトは元のオブジェクトをそのまま
                result.push_back(obj);
                break;
        }
        
        return result;
    }
}
