// ========================================
// EnemyEditorManager.h・医Μ繝輔ぃ繧ｯ繧ｿ繝ｪ繝ｳ繧ｰ迚・- Modify謾ｹ蝟・ｼ・
// ========================================
#pragma once
#include "../Base/EditorManagerBase.h"
#include "EnemyEditorTypes.h"

namespace Jam::Domain::Editor
{
    // 謨ｵ逕ｨ縺ｮ繧ｨ繝・ぅ繧ｿ繧ｪ繝悶ず繧ｧ繧ｯ繝茨ｼ域眠蝓ｺ蠎輔け繝ｩ繧ｹ蟇ｾ蠢懶ｼ・
    struct EnemyEditorObjectNew : EditorObjectBase<EnemyObject>
    {
        using DataType = EnemyObject;
        
        EnemyEditorObjectNew() = default;
        explicit EnemyEditorObjectNew(const EnemyObject& obj)
            : EditorObjectBase<EnemyObject>(obj) {}
    };
    
    // 謨ｵ逕ｨ縺ｮ繧ｳ繝槭Φ繝会ｼ・odify謾ｹ蝟・沿・・
    struct EnemyEditorCommandNew : EditorCommandBase<EnemyEditorObjectNew>
    {
        // 蝓ｺ蠎輔け繝ｩ繧ｹ縺ｮoldData/newData繧剃ｽｿ逕ｨ
    };
    
    // 謨ｵ繧ｨ繝・ぅ繧ｿ繝槭ロ繝ｼ繧ｸ繝｣
    class EnemyEditorManager : public EditorManagerBase<EnemyEditorObjectNew, EnemyEditorCommandNew>
    {
    private:
        using Base = EditorManagerBase<EnemyEditorObjectNew, EnemyEditorCommandNew>;
        
    public:
        // ===== オブジェクト操作 =====
        
        size_t addObject(const EnemyObject& enemy) override;
        void removeObject(size_t index) override;
        void modifyObject(size_t index, const EnemyObject& newEnemy);
        
        // ===== 検索 =====
        
        Optional<size_t> findObjectAt(const Vec2& pos) const;
        bool hasOverlappingEnemy(const Vec2& pos, double minDistance = 50.0) const;
        
        // ===== ファイルI/O =====
        
        void saveToJSON(const FilePath& path) const override;
        void loadFromJSON(const FilePath& path) override;
        
    protected:
        // ===== コマンド実行（Modify改善版） =====
        
        void executeUndoCommand(const EnemyEditorCommandNew& cmd) override;
        void executeRedoCommand(const EnemyEditorCommandNew& cmd) override;
        
    private:
        // ===== JSON変換 =====
        
        JSON enemyObjectToJSON(const EnemyObject& enemy) const;
        Optional<EnemyObject> jsonToEnemyObject(const JSON& json) const;
    };
}