#pragma once
#include <Siv3D.hpp>
#include "../Domain/Stage/Stage.h"
#include "../Infrastructure/Siv3DPhysicsBody.h"

namespace Jam::Presentation {
    
    // Stage定数の定義
    namespace StageConstants {
        constexpr double DefaultGravity = 980.0;
        constexpr double PhysicsStepTime = 1.0 / 200.0;
        constexpr P2BodyType StaticBodyType = P2BodyType::Static;
    }
    
    class StageManager {
    private:
        std::unique_ptr<Domain::Stage::Stage> m_stage;
        Array<std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody>> m_physicsBodies;
        P2World& m_world;
        
    public:
        explicit StageManager(P2World& world) 
            : m_stage(std::make_unique<Domain::Stage::Stage>()), m_world(world) {
        }
        
        bool loadFromJson(const String& jsonPath) {
            if (!m_stage->loadFromJson(jsonPath)) {
                return false;
            }
            
            // 物理ボディを作成
            createPhysicsBodies();
            return true;
        }
        
        void draw() const {
            if (m_stage) {
                m_stage->draw();
            }
        }
        
        const Domain::Stage::Stage* getStage() const {
            return m_stage.get();
        }
        
    private:
        void createPhysicsBodies() {
            clearPhysicsBodies();
            
            if (!m_stage || !m_stage->isLoaded()) {
                return;
            }
            
            const auto& objects = m_stage->getObjects();
            for (const auto& obj : objects) {
                // CollisionType::Noneは物理ボディを作成しない
                if (obj.type == Domain::Stage::CollisionType::None) {
                    continue;
                }
                
                const Vec2 center = obj.rect.center();
                const SizeF size = obj.rect.size;
                
                auto physicsBody = std::make_shared<Infrastructure::Physics::Siv3DPhysicsBody>(
                    m_world, center, size, StageConstants::StaticBodyType
                );
                
                m_physicsBodies.push_back(physicsBody);
            }
        }
        
        void clearPhysicsBodies() {
            m_physicsBodies.clear();
        }
    };
}