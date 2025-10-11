#pragma once
#include "StageTypes.h"

namespace Jam::Domain::Stage {
    class Stage {
    private:
        StageInfo m_info;
        bool m_isLoaded;

    public:
        Stage();
        
        // ステージ管理
        bool loadFromJson(const String& jsonPath);
        bool isLoaded() const { return m_isLoaded; }
        
        // 描画
        void draw() const;
        
    private:
        // JSON解析用ヘルパー
        Array<StageObject> parseObjects(const JSON& objectsJson);
        CollisionType stringToCollisionType(const String& typeStr);
        Color parseColor(const String& colorStr);
    };
}