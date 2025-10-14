#pragma once
#include "StageTypes.h"

namespace Jam::Domain::Stage {
    class CollisionData {
    private:
        Array<StageObject> m_objects;

    public:
        // オブジェクト管理
        void setObjects(const Array<StageObject>& objects) { m_objects = objects; }
        void addObject(const StageObject& object) { m_objects << object; }
        void clearObjects() { m_objects.clear(); }
        
        // 当たり判定クエリ（破壊状態を考慮）
        bool checkCollision(const RectF& rect, const HashSet<String>& destroyedObjects, CollisionType typeFilter = CollisionType::None) const {
            for (const auto& obj : m_objects) {
                if (destroyedObjects.contains(obj.metadata)) continue; // 破壊済みは無視
                if (typeFilter != CollisionType::None && obj.type != typeFilter) continue;
                if (obj.rect.intersects(rect)) return true;
            }
            return false;
        }
        
        Array<StageObject> getCollisions(const RectF& rect, const HashSet<String>& destroyedObjects, CollisionType typeFilter = CollisionType::None) const {
            Array<StageObject> results;
            for (const auto& obj : m_objects) {
                if (destroyedObjects.contains(obj.metadata)) continue; // 破壊済みは無視
                if (typeFilter != CollisionType::None && obj.type != typeFilter) continue;
                if (obj.rect.intersects(rect)) {
                    results << obj;
                }
            }
            return results;
        }
        
        // 従来の当たり判定（破壊状態を考慮しない）
        bool checkCollision(const RectF& rect, CollisionType typeFilter = CollisionType::None) const {
            static const HashSet<String> emptyDestroyed;
            return checkCollision(rect, emptyDestroyed, typeFilter);
        }
        
        Array<StageObject> getCollisions(const RectF& rect, CollisionType typeFilter = CollisionType::None) const {
            static const HashSet<String> emptyDestroyed;
            return getCollisions(rect, emptyDestroyed, typeFilter);
        }
        
        // アクセサ
        const Array<StageObject>& getObjects() const { return m_objects; }
        size_t getObjectCount() const { return m_objects.size(); }
    };
}