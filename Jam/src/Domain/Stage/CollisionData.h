#pragma once
#include "StageTypes.h"

namespace Jam::Domain::Stage {
    class CollisionData {
    private:
        Array<StageObject> m_objects;

    public:
        // オブジェクト管理
        void setObjects(const Array<StageObject>& objects);
        void addObject(const StageObject& object);
        void clearObjects();
        
        // 当たり判定クエリ
        bool checkCollision(const RectF& rect, CollisionType typeFilter = CollisionType::None) const;
        Array<StageObject> getCollisions(const RectF& rect, CollisionType typeFilter = CollisionType::None) const;
        
        // アクセサ
        const Array<StageObject>& getObjects() const { return m_objects; }
        size_t getObjectCount() const { return m_objects.size(); }
    };
}