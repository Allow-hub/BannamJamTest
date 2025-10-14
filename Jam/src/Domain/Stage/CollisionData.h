#pragma once
#include "StageTypes.h"

namespace Jam::Domain::Stage {
    class CollisionData {
    private:
        Array<StageObject> m_objects;

    public:
        // オブジェクト管理
        void setObjects(const Array<StageObject>& objects) { 
            m_objects = objects; 
        }
        
        void addObject(const StageObject& object) { 
            m_objects << object; 
        }
        
        void clearObjects() { 
            m_objects.clear(); 
        }
        
        // 当たり判定（統一インターフェース）
        bool checkCollision(
            const RectF& rect, 
            CollisionType typeFilter = CollisionType::None,
            const HashSet<String>& destroyedObjects = {}
        ) const {
            return !getCollisions(rect, typeFilter, destroyedObjects).empty();
        }
        
        Array<StageObject> getCollisions(
            const RectF& rect, 
            CollisionType typeFilter = CollisionType::None,
            const HashSet<String>& destroyedObjects = {}
        ) const {
            Array<StageObject> results;
            results.reserve(8); // 一般的な衝突数を想定した事前確保
            
            for (const auto& obj : m_objects) {
                if (!shouldCheckObject(obj, typeFilter, destroyedObjects)) continue;
                if (obj.rect.intersects(rect)) {
                    results << obj;
                }
            }
            
            return results;
        }
        
        // アクセサ
        const Array<StageObject>& getObjects() const { 
            return m_objects; 
        }
        
        size_t getObjectCount() const { 
            return m_objects.size(); 
        }
        
    private:
        // フィルタリングロジックの統一
        bool shouldCheckObject(
            const StageObject& obj,
            CollisionType typeFilter,
            const HashSet<String>& destroyedObjects
        ) const {
            if (destroyedObjects.contains(obj.metadata)) return false;
            if (typeFilter != CollisionType::None && obj.type != typeFilter) return false;
            return true;
        }
    };
}