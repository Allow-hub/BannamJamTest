#pragma once
#include "StageTypes.h"
#include "CollisionData.h"
#include "../../Infrastructure/StageLoader.h"

namespace Jam::Domain::Stage {
    class Stage {
    private:
        CollisionData m_collisionData;
        HashSet<String> m_destroyedObjects;
        bool m_isLoaded = false;

    public:
        Stage() = default;
        
        // JSON読み込み（Infrastructure::StageLoaderに委譲）
        bool loadFromJson(const String& jsonPath) {
            Array<StageObject> objects;
            if (!Infrastructure::Stage::StageLoader::loadFromJson(jsonPath, objects)) {
                return false;
            }
            
            m_collisionData.setObjects(objects);
            m_destroyedObjects.clear();
            m_isLoaded = true;
            return true;
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
            return m_collisionData.checkCollision(rect, m_destroyedObjects, typeFilter);
        }
        
        Array<StageObject> getCollisions(const RectF& rect, CollisionType typeFilter = CollisionType::None) const {
            return m_collisionData.getCollisions(rect, m_destroyedObjects, typeFilter);
        }
        
        // アクセサ
        const Array<StageObject>& getObjects() const { return m_collisionData.getObjects(); }
        const CollisionData& getCollisionData() const { return m_collisionData; }
        const HashSet<String>& getDestroyedObjects() const { return m_destroyedObjects; }
        
        // 描画
        void draw() const {
            if (!m_isLoaded) return;
            
            for (const auto& obj : m_collisionData.getObjects()) {
                if (!isObjectDestroyed(obj.metadata)) {
                    obj.rect.draw(obj.color);
                    
                    // 破壊可能なオブジェクトには枠を表示
                    if (obj.destructible) {
                        obj.rect.drawFrame(2, Palette::Red);
                    }
                }
            }
        }
    };
}
