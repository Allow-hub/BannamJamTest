#pragma once
#include "../Base/EditorManagerBase.h"
#include "EnemyEditorTypes.h"

namespace Jam::Domain::Editor
{
    // 敵用のエディタオブジェクト
    struct EnemyEditorObject : EditorObjectBase<EnemyObject>
    {
        using DataType = EnemyObject;
        
        EnemyEditorObject() = default;
        explicit EnemyEditorObject(const EnemyObject& obj)
            : EditorObjectBase<EnemyObject>(obj) {}
    };
    
    // 敵用のコマンド
    struct EnemyEditorCommand : EditorCommandBase<EnemyEditorObject>
    {
        // 基底クラスのoldData/newDataを使用するため空実装
    };
    
    // 敵エディタマネージャー
    class EnemyEditorManager : public EditorManagerBase<EnemyEditorObject, EnemyEditorCommand>
    {
    private:
        using Base = EditorManagerBase<EnemyEditorObject, EnemyEditorCommand>;
        
    public:
        static constexpr double DEFAULT_MIN_DISTANCE = 50.0;
        
        // ===== オブジェクト操作 =====
        
        size_t addObject(const EnemyObject& enemy) override;
        void removeObject(size_t index) override;
        void modifyObject(size_t index, const EnemyObject& newEnemy);
        
        // ===== 検索 =====
        
        Optional<size_t> findObjectAt(const Vec2& pos) const;
        bool hasOverlappingEnemy(const Vec2& pos, double minDistance = DEFAULT_MIN_DISTANCE) const;
        
        // ===== ファイルI/O =====
        
        void saveToJSON(const FilePath& path) const override;
        void loadFromJSON(const FilePath& path) override;
        
    protected:
        // ===== コマンド実行 =====
        
        void executeUndoCommand(const EnemyEditorCommand& cmd) override;
        void executeRedoCommand(const EnemyEditorCommand& cmd) override;
        
    private:
        // ===== JSON変換 =====
        
        JSON enemyObjectToJSON(const EnemyObject& enemy) const;
        Optional<EnemyObject> jsonToEnemyObject(const JSON& json) const;
        
        // ===== JSONパースヘルパー =====
        
        void parsePatrolAI(EnemyObject& enemy, const JSON& aiJson) const;
    };
}