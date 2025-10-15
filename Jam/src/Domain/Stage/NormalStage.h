#pragma once
#include "IStage.h"
#include "StageTypes.h"
#include "../Physics/IPhysicsBody.h"

namespace Jam::Domain::Stage {
    
    class NormalStage : public IStage {
    private:
        struct StagePhysicsObject {
            StageObject visualData;
            std::shared_ptr<Physics::IPhysicsBody> physicsBody;
        };
        
        Array<StagePhysicsObject> m_objects;
        HashSet<String> m_destroyedObjects;
        bool m_isLoaded = false;

    public:
        // IPhysicsBodyの配列を注入
        NormalStage() = default;
        explicit NormalStage(const Array<std::shared_ptr<Physics::IPhysicsBody>>& physicsBodies) {
            // 現在は空実装
            m_isLoaded = true;
        }
        
        // GameScene.hで使用されているメソッド
        using PhysicsBodyFactory = std::function<std::shared_ptr<Physics::IPhysicsBody>(const RectF&, Physics::PhysicsLayer)>;
        
        void setObjects(const Array<StageObject>& objects, PhysicsBodyFactory bodyFactory) {
            m_objects.clear();
            m_destroyedObjects.clear();
            
            for (const auto& obj : objects) {
                // 当たり判定が必要なタイプのみ物理ボディを作成
                if (obj.type == StageType::Solid || 
                    obj.type == StageType::Platform ||
                    obj.type == StageType::Breakable) {
                    
                    auto physicsBody = bodyFactory(obj.rect, Physics::PhysicsLayer::Ground);
                    m_objects.push_back({obj, physicsBody});
                } else {
                    // 物理ボディが不要なオブジェクト（装飾等）
                    m_objects.push_back({obj, nullptr});
                }
            }
            
            m_isLoaded = true;
        }
        
        bool isLoaded() const { 
            return m_isLoaded; 
        }
        
        bool destroyObject(const String& objectId) override {
            for (auto& obj : m_objects) {
                if (obj.visualData.metadata == objectId) {
                    m_destroyedObjects.insert(objectId);
                    // 物理ボディを無効化（削除ではなく無効化）
                    if (obj.physicsBody) {
                        obj.physicsBody->setLayer(Physics::PhysicsLayer::None);
                    }
                    return true; // 破壊成功
                }
            }
            return false; // 破壊失敗
        }
        
        bool isObjectDestroyed(const String& objectId) const override {
            return m_destroyedObjects.contains(objectId);
        }
        
        Array<std::shared_ptr<Physics::IPhysicsBody>> getPhysicsBodies() const override {
            Array<std::shared_ptr<Physics::IPhysicsBody>> bodies;
            for (const auto& obj : m_objects) {
                if (obj.physicsBody && !isObjectDestroyed(obj.visualData.metadata)) {
                    bodies.push_back(obj.physicsBody);
                }
            }
            return bodies;
        }
        
        Array<StageObject> getRenderableObjects() const override {
            Array<StageObject> renderable;
            renderable.reserve(m_objects.size() - m_destroyedObjects.size());
            
            for (const auto& obj : m_objects) {
                if (!isObjectDestroyed(obj.visualData.metadata)) {
                    renderable.push_back(obj.visualData);
                }
            }
            return renderable;
        }
        
    };
}
