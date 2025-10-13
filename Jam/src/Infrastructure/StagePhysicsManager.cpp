#include "StagePhysicsManager.h"

namespace Jam::Infrastructure {
    
    StagePhysicsManager::StagePhysicsManager(P2World& world) 
        : m_world(world) {
    }
    
    void StagePhysicsManager::createPhysicsBodies(const Array<Domain::Stage::StageObject>& objects) {
        clearPhysicsBodies();
        
        for (const auto& obj : objects) {
            // CollisionType::Noneは物理ボディを作成しない
            if (obj.type == Domain::Stage::CollisionType::None) {
                continue;
            }
            
            // 物理ボディを作成
            Vec2 center = obj.rect.center();
            SizeF size = obj.rect.size;
            P2BodyType bodyType = getBodyType(obj.type);
            
            auto physicsBody = std::make_shared<Physics::Siv3DPhysicsBody>(
                m_world, center, size, bodyType
            );
            
            m_physicsBodies.push_back(physicsBody);
            
            Print << U"Created physics body for: " << obj.metadata 
                  << U" at (" << center.x << U", " << center.y << U")";
        }
        
        Print << U"Created " << m_physicsBodies.size() << U" physics bodies for stage";
    }
    
    void StagePhysicsManager::clearPhysicsBodies() {
        m_physicsBodies.clear();
    }
    
    const Array<std::shared_ptr<Domain::Physics::IPhysicsBody>>& StagePhysicsManager::getPhysicsBodies() const {
        // キャスト用の静的配列を返す（実装上の妥協）
        static Array<std::shared_ptr<Domain::Physics::IPhysicsBody>> result;
        result.clear();
        for (const auto& body : m_physicsBodies) {
            result.push_back(std::static_pointer_cast<Domain::Physics::IPhysicsBody>(body));
        }
        return result;
    }
    
    P2BodyType StagePhysicsManager::getBodyType(Domain::Stage::CollisionType type) const {
        switch (type) {
            case Domain::Stage::CollisionType::Solid:
            case Domain::Stage::CollisionType::Platform:
                return P2BodyType::Static;  // 動かない床・壁
            case Domain::Stage::CollisionType::Hazard:
            case Domain::Stage::CollisionType::Trigger:
                return P2BodyType::Static;  // センサー的な用途だが、まずは静的で
            default:
                return P2BodyType::Static;
        }
    }
}