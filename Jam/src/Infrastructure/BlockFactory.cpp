#include "Infrastructure/BlockFactory.h"
#include "Domain/Block/NormalBlock.h"
#include "Domain/Block/MovingBlock.h"
#include "Domain/Block/OneWayBlock.h"
#include "Domain/Block/DamageBlock.h"
#include "Domain/Block/MovingDamageBlock.h"
#include "Domain/Events/GameEvents.h"
#include "Infrastructure/BlockLoader.h"
#include "Infrastructure/PhysicsFilterManager.h"

namespace Jam::Infrastructure {
    
    BlockCreationResult BlockFactory::createBlocksFromFile(
        const String& filename,
        std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory,
        Domain::Events::GameEventQueue& eventQueue,
        Domain::Physics::PhysicsBodyID playerId
    ) {
        BlockCreationResult result;
        
        Array<Domain::Block::BlockObject> objects;
        if (!Jam::Infrastructure::Block::BlockLoader::loadBlockFromFile(filename, objects)) {
            return result;
        }
        
        for (const auto& obj : objects) {
            auto createdObjects = expandObjectByGroundSide(obj);
            
            Array<size_t> bodyIndicesForThisStage;
            Vec2 baseCenter = obj.rect.center();
            
            if (!createdObjects.isEmpty()) {
                std::shared_ptr<Domain::Physics::IPhysicsBody> body;
                auto stage = createBlock(createdObjects[0], bodyFactory, body, eventQueue, playerId);
                
                if (stage && body) {
                    size_t bodyIndex = result.physicsBodies.size();
                    result.physicsBodies.push_back(body);
                    bodyIndicesForThisStage.push_back(bodyIndex);
                    
                    Vec2 offset = createdObjects[0].rect.center() - baseCenter;
                    result.bodyOffsets.push_back(offset);
                    result.bodyGroundSides.push_back(createdObjects[0].groundSide);
                    
                    // 追加の物理ボディを処理（groundSide展開時）
                    // ※ダメージ床にはgroundSideがないため、このループは通常のステージのみ
                    for (size_t i = 1; i < createdObjects.size(); ++i) {
                        std::shared_ptr<Domain::Physics::IPhysicsBody> additionalBody;
                        createBlock(createdObjects[i], bodyFactory, additionalBody, eventQueue, playerId);
                        if (additionalBody) {
                            size_t additionalBodyIndex = result.physicsBodies.size();
                            result.physicsBodies.push_back(additionalBody);
                            bodyIndicesForThisStage.push_back(additionalBodyIndex);
                            
                            Vec2 additionalOffset = createdObjects[i].rect.center() - baseCenter;
                            result.bodyOffsets.push_back(additionalOffset);
                            result.bodyGroundSides.push_back(createdObjects[i].groundSide);
                        }
                    }
                    
                    result.stages.push_back(std::move(stage));
                    result.bodyIndices.push_back(bodyIndicesForThisStage);
                }
            }
        }
        
        return result;
    }
    
    std::unique_ptr<Domain::Block::IBlock> BlockFactory::createBlock(
        const Domain::Block::BlockObject& obj,
        std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory,
        std::shared_ptr<Domain::Physics::IPhysicsBody>& outBody,
        Domain::Events::GameEventQueue& eventQueue,
        Domain::Physics::PhysicsBodyID playerId
    ) {
        if (!bodyFactory) {
            return nullptr;
        }
        
        auto physicsLayer = getPhysicsLayerFromType(obj.type, obj.groundSide);
        
        P2BodyType bodyType = (obj.type == Domain::Block::BlockType::MovingPlatform || 
                               obj.type == Domain::Block::BlockType::MovingDamagePlatform)
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
        if (obj.type == Domain::Block::BlockType::OneWayPlatform) {
            outBody->setFilter(Jam::Infrastructure::PhysicsFilter::OneWayPlatform);
        }
        
        switch (obj.type) {
            case Domain::Block::BlockType::MovingPlatform:
                return std::make_unique<Domain::Block::MovingBlock>(obj);
                
            case Domain::Block::BlockType::OneWayPlatform:
                return std::make_unique<Domain::Block::OneWayBlock>(obj);
                
            case Domain::Block::BlockType::DamagePlatform:
            {
                auto damageStage = std::make_unique<Domain::Block::DamageBlock>(obj, outBody, eventQueue, playerId);
                damageStage->init();  // unique_ptrのまま、生ポインタでinit()を呼ぶ
                return damageStage;
            }
                
            case Domain::Block::BlockType::MovingDamagePlatform:
            {
                auto movingDamageStage = std::make_unique<Domain::Block::MovingDamageBlock>(obj, outBody, eventQueue, playerId);
                movingDamageStage->init();  // unique_ptrのまま、生ポインタでinit()を呼ぶ
                return movingDamageStage;
            }
                
            case Domain::Block::BlockType::Normal:
            default:
                return std::make_unique<Domain::Block::NormalBlock>(obj);
        }
    }
    
    Domain::Physics::PhysicsLayer BlockFactory::getPhysicsLayerFromType(
        Domain::Block::BlockType type, 
        Domain::Block::GroundSide groundSide
    ) {
        using namespace Domain::Physics;
        
        // OneWayPlatformの場合
        if (type == Domain::Block::BlockType::OneWayPlatform) {
            if (groundSide == Domain::Block::GroundSide::None) {
                // 本体: OneWayPlatformレイヤー(青色)
                return PhysicsLayer::OneWayPlatform;
            } else {
                // 上面: Groundレイヤー(緑色)
                return PhysicsLayer::Ground;
            }
        }
        
        // 通常のステージ
        if (groundSide == Domain::Block::GroundSide::None) {
            return PhysicsLayer::Wall;
        }
        return PhysicsLayer::Ground;
    }

    Array<Domain::Block::BlockObject> BlockFactory::expandObjectByGroundSide(const Domain::Block::BlockObject& obj) {
        using namespace Domain::Block;
        Array<BlockObject> result;

        constexpr double groundThickness = 5.0;
        
		if (obj.groundSide == GroundSide::All) {
			// 中央の壁部分
			BlockObject wallObj = obj;
			wallObj.groundSide = GroundSide::None;
			result.push_back(wallObj);

			// 左面（全高さ）
			BlockObject leftGround = obj;
			leftGround.groundSide = GroundSide::Left;
			leftGround.rect = RectF(
				obj.rect.x,
				obj.rect.y,
				groundThickness,
				obj.rect.h
			);
			result.push_back(leftGround);

			// 右面（全高さ）
			BlockObject rightGround = obj;
			rightGround.groundSide = GroundSide::Right;
			rightGround.rect = RectF(
				obj.rect.x + obj.rect.w - groundThickness,
				obj.rect.y,
				groundThickness,
				obj.rect.h
			);
			result.push_back(rightGround);

			// 上面（左右のgroundThicknessを除く）
			BlockObject topGround = obj;
			topGround.groundSide = GroundSide::Up;
			topGround.rect = RectF(
				obj.rect.x + groundThickness,
				obj.rect.y,
				obj.rect.w - groundThickness * 2,
				groundThickness
			);
			result.push_back(topGround);

			// 下面（左右のgroundThicknessを除く）
			BlockObject bottomGround = obj;
			bottomGround.groundSide = GroundSide::Down;
			bottomGround.rect = RectF(
				obj.rect.x + groundThickness,
				obj.rect.y + obj.rect.h - groundThickness,
				obj.rect.w - groundThickness * 2,
				groundThickness
			);
			result.push_back(bottomGround);

			return result;
		}

		// None以外の場合、壁部分を追加
		if (obj.groundSide != GroundSide::None) {
			BlockObject wallObj = obj;
			wallObj.groundSide = GroundSide::None;
			result.push_back(wallObj);
		}

		// 地面判定を作成
		BlockObject groundObj = obj;

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
