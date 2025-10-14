#pragma once
#include "StageTypes.h"
#include "IStageLoader.h"
#include "IStagePhysicsManager.h"

namespace Jam::Domain::Stage {
    class Stage {
    private:
        StageInfo m_info;
        bool m_isLoaded;
        HashSet<String> m_destroyedObjects;  // 破壊されたオブジェクトのID

    public:
        Stage();
        
        // ステージ管理（依存性注入でローダーと物理マネージャーを受け取る）
        bool loadFromJson(const String& jsonPath, IStageLoader& loader, IStagePhysicsManager& physicsManager);
        bool isLoaded() const { return m_isLoaded; }
        
        // ゲッター
        const StageInfo& getStageInfo() const { return m_info; }
        const Array<StageObject>& getObjects() const { return m_info.objects; }
        
        // 破壊機能
        void destroyObject(const String& objectId);
        bool isObjectDestroyed(const String& objectId) const;
        void resetDestroyedObjects();  // 破壊状態をリセット
        
        // 描画
        void draw() const;
    };
}