#pragma once
#include "StageTypes.h"
#include "CollisionData.h"

namespace Jam::Domain::Stage {
    class Stage {
    private:
        CollisionData m_collisionData;
        HashSet<String> m_destroyedObjects;
        bool m_isLoaded = false;

    public:
        Stage() = default;
        
        void setObjects(const Array<StageObject>& objects) {
            m_collisionData.setObjects(objects);
            m_destroyedObjects.clear();
            m_isLoaded = true;
        }
        
        bool isLoaded() const { return m_isLoaded; }
        
        // 破壊機能
        void destroyObject(const String& objectId) {
            for (const auto& obj : m_collisionData.getObjects()) {
                if (obj.metadata == objectId && obj.destructible) {
                    m_destroyedObjects.insert(objectId);
                    break;
                }
            }
        }
        
        bool isObjectDestroyed(const String& objectId) const {
            return m_destroyedObjects.contains(objectId);
        }
        
        void resetDestroyedObjects() {
            m_destroyedObjects.clear();
        }
        
        // 当たり判定アクセス（破壊状態考慮）
        bool checkCollision(const RectF& rect, CollisionType typeFilter = CollisionType::None) const {
            return m_collisionData.checkCollision(rect, typeFilter, m_destroyedObjects);
        }
        
        Array<StageObject> getCollisions(const RectF& rect, CollisionType typeFilter = CollisionType::None) const {
            return m_collisionData.getCollisions(rect, typeFilter, m_destroyedObjects);
        }
        
        // アクセサ
        const Array<StageObject>& getObjects() const { 
            return m_collisionData.getObjects(); 
        }
        
        const CollisionData& getCollisionData() const { 
            return m_collisionData; 
        }
        
        const HashSet<String>& getDestroyedObjects() const { 
            return m_destroyedObjects; 
        }
        
        size_t getObjectCount() const {
            return m_collisionData.getObjectCount();
        }
        
        // 描画データ取得（描画責任はPresentation層へ移譲）
        Array<StageObject> getRenderableObjects() const {
            Array<StageObject> renderable;
            for (const auto& obj : m_collisionData.getObjects()) {
                if (!isObjectDestroyed(obj.metadata)) {
                    renderable << obj;
                }
            }
            return renderable;
        }
    };
}
