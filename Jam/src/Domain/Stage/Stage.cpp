#include "Stage.h"

namespace Jam::Domain::Stage {
    
    Stage::Stage() : m_isLoaded(false) {
        // 初期化
    }
    
    bool Stage::loadFromJson(const String& jsonPath, IStageLoader& loader, IStagePhysicsManager& physicsManager) {
        m_isLoaded = false;
        
        // ローダーを使ってステージデータを読み込み
        if (!loader.loadStageData(jsonPath, m_info)) {
            return false;
        }
        
        // 物理マネージャーを使って物理ボディを作成
        physicsManager.createPhysicsBodies(m_info.objects);
        
        m_isLoaded = true;
        return true;
    }
    
    void Stage::draw() const {
        if (!m_isLoaded) {
            return;
        }
        
        // 全オブジェクトを描画
        for (const auto& obj : m_info.objects) {
            obj.rect.draw(obj.color);
        }
    }
}