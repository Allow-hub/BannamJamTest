// ========================================
// StageEditorManager.h・医Μ繝輔ぃ繧ｯ繧ｿ繝ｪ繝ｳ繧ｰ迚・- Modify謾ｹ蝟・ｼ・
// ========================================
#pragma once
#include "../Base/EditorManagerBase.h"
#include "StageEditorTypes.h"
#include "../../Stage/StageTypes.h"

namespace Jam::Domain::Editor
{
    // ステージ用のエディタオブジェクト（新基底クラス対応）
    struct StageEditorObjectNew : EditorObjectBase<Stage::StageObject>
    {
        using DataType = Stage::StageObject;
        
        StageEditorObjectNew() = default;
        explicit StageEditorObjectNew(const Stage::StageObject& obj) 
            : EditorObjectBase<Stage::StageObject>(obj) {}
    };
    
    // ステージ用のコマンド（Modify改善版）
    struct StageEditorCommandNew : EditorCommandBase<StageEditorObjectNew>
    {
        // 基底クラスのoldData/newDataを使用
    };
    
    // ステージエディタマネージャー
    class StageEditorManager : public EditorManagerBase<StageEditorObjectNew, StageEditorCommandNew>
    {
    private:
        using Base = EditorManagerBase<StageEditorObjectNew, StageEditorCommandNew>;
        
    public:
        // ===== オブジェクト操作 =====
        
        size_t addObject(const Stage::StageObject& obj) override;
        void removeObject(size_t id) override;
        void moveObject(size_t id, const Vec2& newPos);
        void modifyObject(size_t id, const Stage::StageObject& newObj);
        
        // ===== 驕ｸ謚槭が繝悶ず繧ｧ繧ｯ繝医・荳諡ｬ譖ｴ譁ｰ =====
        
        void updateSelectedObjectsMovement(double distance, double speed, Stage::MovementType type);
        void updateSelectedObjectsDamage(double damage);
        
        // ===== 検索 =====
        
        Optional<size_t> findObjectAt(const Vec2& pos) const;
        bool hasObjectAtExactPosition(const RectF& rect) const;
        bool hasOverlappingObject(const RectF& rect) const;
        
        // ===== ファイルI/O =====
        
        void saveToJSON(const FilePath& path) const override;
        void loadFromJSON(const FilePath& path) override;
        
    protected:
        // ===== コマンド実行（Modify改善版） =====
        
        void executeUndoCommand(const StageEditorCommandNew& cmd) override;
        void executeRedoCommand(const StageEditorCommandNew& cmd) override;
        
    private:
        // ===== ユーティリティ =====
        
        Array<Stage::StageObject> mergeAdjacentObjects() const;
        JSON stageObjectToJSON(const Stage::StageObject& obj) const;
        Optional<Stage::StageObject> jsonToStageObject(const JSON& json) const;
    };
}