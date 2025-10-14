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
        
        // 破壊状態をリセット
        m_destroyedObjects.clear();
        
        m_isLoaded = true;
        return true;
    }
    
    void Stage::destroyObject(const String& objectId) {
        // 破壊可能なオブジェクトかチェック
        for (const auto& obj : m_info.objects) {
            if (obj.metadata == objectId && obj.destructible) {
                m_destroyedObjects.insert(objectId);
                break;
            }
        }
    }
    
    bool Stage::isObjectDestroyed(const String& objectId) const {
        return m_destroyedObjects.contains(objectId);
    }
    
    void Stage::resetDestroyedObjects() {
        m_destroyedObjects.clear();
    }
    
    void Stage::draw() const {
        if (!m_isLoaded) {
            return;
        }
        
        // 破壊されていないオブジェクトのみを描画
        for (const auto& obj : m_info.objects) {
            if (!isObjectDestroyed(obj.metadata)) {
                obj.rect.draw(obj.color);
                
                // 破壊可能なオブジェクトには枠を表示
                if (obj.destructible) {
                    obj.rect.drawFrame(2, Palette::Red);
                }
            }
        }
    }
}