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
            
            // このステージに属する物理ボディのインデックスを記録
            Array<size_t> bodyIndicesForThisStage;
            
            // 元のオブジェクトの中心位置（基準位置）
            Vec2 baseCenter = obj.rect.center();
            
            // 最初のオブジェクトからStageを生成
            if (!createdObjects.isEmpty()) {
                std::shared_ptr<Domain::Physics::IPhysicsBody> body;
                auto stage = createStage(createdObjects[0], bodyFactory, body);
                
                if (stage && body) {
                    size_t bodyIndex = result.physicsBodies.size();
                    result.physicsBodies.push_back(body);
                    bodyIndicesForThisStage.push_back(bodyIndex);
                    
                    // 基準位置からのオフセットを計算
                    Vec2 offset = createdObjects[0].rect.center() - baseCenter;
                    result.bodyOffsets.push_back(offset);
                    
                    // 残りのオブジェクトは物理ボディのみ生成（同じStageで制御）
                    for (size_t i = 1; i < createdObjects.size(); ++i) {
                        std::shared_ptr<Domain::Physics::IPhysicsBody> additionalBody;
                        createStage(createdObjects[i], bodyFactory, additionalBody);
                        if (additionalBody) {
                            size_t additionalBodyIndex = result.physicsBodies.size();
                            result.physicsBodies.push_back(additionalBody);
                            bodyIndicesForThisStage.push_back(additionalBodyIndex);
                            
                            // このボディのオフセットも計算
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
        
        // GroundSideも考慮してPhysicsLayerを決定
        auto physicsLayer = getPhysicsLayerFromType(obj.type, obj.groundSide);
        
        // 動く床はKinematicボディ、それ以外はStaticボディ
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
    
    Domain::Physics::PhysicsLayer StageFactory::getPhysicsLayerFromType(
        Domain::Stage::StageType type, 
        Domain::Stage::GroundSide groundSide
    ) {
        using namespace Domain::Physics;
        
        // GroundSide::Noneの場合は常にWallレイヤー
        if (groundSide == Domain::Stage::GroundSide::None) {
            return PhysicsLayer::Wall;
        }
        
        switch (type) {
            case Domain::Stage::StageType::Normal:
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
            wallObj.groundSide = GroundSide::None;  // 再帰防止
            result.push_back(wallObj);
        }
        
        // 指定された面に薄い地上判定を追加
        StageObject groundObj = obj;
        groundObj.type = StageType::Normal;  // 床として扱う
        groundObj.groundSide = GroundSide::Up;  // Groundレイヤーになるように明示的にUpを設定
        
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
