#include "StageFactory.h"

namespace Jam::Infrastructure {
    
    StageCreationResult StageFactory::createStagesFromFile(
        const String& filename,
        std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory,
        Domain::Events::GameEventQueue& eventQueue,
        Domain::Physics::PhysicsBodyID playerId
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
                auto stage = createStage(createdObjects[0], bodyFactory, body, eventQueue, playerId);
                
                if (stage && body) {
                    size_t bodyIndex = result.physicsBodies.size();
                    result.physicsBodies.push_back(body);
                    bodyIndicesForThisStage.push_back(bodyIndex);
                    
                    Vec2 offset = createdObjects[0].rect.center() - baseCenter;
                    result.bodyOffsets.push_back(offset);
                    
                    for (size_t i = 1; i < createdObjects.size(); ++i) {
                        std::shared_ptr<Domain::Physics::IPhysicsBody> additionalBody;
                        createStage(createdObjects[i], bodyFactory, additionalBody, eventQueue, playerId);
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
        std::shared_ptr<Domain::Physics::IPhysicsBody>& outBody,
        Domain::Events::GameEventQueue& eventQueue,
        Domain::Physics::PhysicsBodyID playerId
    ) {
        if (!bodyFactory) {
            return nullptr;
        }
        
        auto physicsLayer = getPhysicsLayerFromType(obj.type, obj.groundSide);
        
        P2BodyType bodyType = (obj.type == Domain::Stage::StageType::MovingPlatform || 
                               obj.type == Domain::Stage::StageType::MovingDamagePlatform)
            ? P2BodyType::Kinematic
            : P2BodyType::Static;
        
        outBody = bodyFactory->createBody(
            obj.rect.center(),
            obj.rect.size,
            bodyType,
            Domain::Physics::PhysicsMaterial{ 0.0, 0.0, 0.0 }
        );
        
        if (!outBody) {
            return nullptr;
        }
        
        outBody->setLayer(physicsLayer);
        
        // OneWayPlatform全体にすり抜けフィルターを適用
        // 着地判定は手動で行う
        if (obj.type == Domain::Stage::StageType::OneWayPlatform) {
            outBody->setFilter(Jam::Infrastructure::PhysicsFilter::OneWayPlatform);
        }
        
        switch (obj.type) {
            case Domain::Stage::StageType::MovingPlatform:
                return std::make_unique<Domain::Stage::MovingPlatformStage>(obj);
                
            case Domain::Stage::StageType::OneWayPlatform:
                return std::make_unique<Domain::Stage::OneWayPlatformStage>(obj);
                
            case Domain::Stage::StageType::DamagePlatform:
            {
                auto damageStage = std::make_unique<Domain::Stage::DamageStage>(obj, outBody, eventQueue, playerId);
                damageStage->init();  // unique_ptrのまま、生ポインタでinit()を呼ぶ
                return damageStage;
            }
                
            case Domain::Stage::StageType::MovingDamagePlatform:
            {
                auto movingDamageStage = std::make_unique<Domain::Stage::MovingDamagePlatformStage>(obj, outBody, eventQueue, playerId);
                movingDamageStage->init();  // unique_ptrのまま、生ポインタでinit()を呼ぶ
                return movingDamageStage;
            }
                
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
        
        // OneWayPlatformの場合
        if (type == Domain::Stage::StageType::OneWayPlatform) {
            if (groundSide == Domain::Stage::GroundSide::None) {
                // 本体: OneWayPlatformレイヤー(青色)
                return PhysicsLayer::OneWayPlatform;
            } else {
                // 上面: Groundレイヤー(緑色)
                return PhysicsLayer::Ground;
            }
        }
        
        // 通常のステージ
        if (groundSide == Domain::Stage::GroundSide::None) {
            return PhysicsLayer::Wall;
        }
        return PhysicsLayer::Ground;
    }

    Array<Domain::Stage::StageObject> StageFactory::expandObjectByGroundSide(const Domain::Stage::StageObject& obj) {
        using namespace Domain::Stage;
        Array<StageObject> result;

        constexpr double groundThickness = 5.0;
        
		if (obj.groundSide == GroundSide::All) {
			// 中央の壁部分
			StageObject wallObj = obj;
			wallObj.groundSide = GroundSide::None;
			result.push_back(wallObj);

			// 上面
			StageObject topGround = obj;
			topGround.groundSide = GroundSide::Up;
			topGround.rect = RectF(
				obj.rect.x,
				obj.rect.y,
				obj.rect.w,
				groundThickness
			);
			result.push_back(topGround);

			// 下面
			StageObject bottomGround = obj;
			bottomGround.groundSide = GroundSide::Down;
			bottomGround.rect = RectF(
				obj.rect.x,
				obj.rect.y + obj.rect.h - groundThickness,
				obj.rect.w,
				groundThickness
			);
			result.push_back(bottomGround);

			// 左面
			StageObject leftGround = obj;
			leftGround.groundSide = GroundSide::Left;
			leftGround.rect = RectF(
				obj.rect.x,
				obj.rect.y,
				groundThickness,
				obj.rect.h
			);
			result.push_back(leftGround);

			// 右面
			StageObject rightGround = obj;
			rightGround.groundSide = GroundSide::Right;
			rightGround.rect = RectF(
				obj.rect.x + obj.rect.w - groundThickness,
				obj.rect.y,
				groundThickness,
				obj.rect.h
			);
			result.push_back(rightGround);

			return result;
		}

		// None以外の場合、壁部分を追加
		if (obj.groundSide != GroundSide::None) {
			StageObject wallObj = obj;
			wallObj.groundSide = GroundSide::None;
			result.push_back(wallObj);
		}

		// 地面判定を作成
		StageObject groundObj = obj;

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

		case GroundSide::Down:
			groundObj.rect = RectF(
				obj.rect.x,
				obj.rect.y + obj.rect.h - groundThickness,
				obj.rect.w,
				groundThickness
			);
			result.push_back(groundObj);
			break;

		case GroundSide::Left:
			groundObj.rect = RectF(
				obj.rect.x,
				obj.rect.y,
				groundThickness,
				obj.rect.h
			);
			result.push_back(groundObj);
			break;

		case GroundSide::Right:
			groundObj.rect = RectF(
				obj.rect.x + obj.rect.w - groundThickness,
				obj.rect.y,
				groundThickness,
				obj.rect.h
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
