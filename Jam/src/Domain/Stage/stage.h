#pragma once
#include "StageTypes.h"
#include "IStageLoader.h"
#include "IStagePhysicsManager.h"

namespace Jam::Domain::Stage {
    class Stage {
    private:
        StageInfo m_info;
        bool m_isLoaded;

    public:
        Stage();
        
        // ステージ管理（依存性注入でローダーと物理マネージャーを受け取る）
        bool loadFromJson(const String& jsonPath, IStageLoader& loader, IStagePhysicsManager& physicsManager);
        bool isLoaded() const { return m_isLoaded; }
        
        // ゲッター
        const StageInfo& getStageInfo() const { return m_info; }
        const Array<StageObject>& getObjects() const { return m_info.objects; }
        
        // 描画
        void draw() const;
    };
}