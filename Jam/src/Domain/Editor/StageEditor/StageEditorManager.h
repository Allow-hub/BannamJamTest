#pragma once
#include "../Base/EditorManagerBase.h"
#include "StageEditorTypes.h"
#include "../../Stage/StageTypes.h"

namespace Jam::Domain::Editor
{
    // ステージ用のエディタオブジェクト
    struct StageEditorObject : EditorObjectBase<Stage::StageObject>
    {
        using DataType = Stage::StageObject;
        
        StageEditorObject() = default;
        explicit StageEditorObject(const Stage::StageObject& obj) 
            : EditorObjectBase<Stage::StageObject>(obj) {}
    };
    
    // ステージ用のコマンド
    struct StageEditorCommand : EditorCommandBase<StageEditorObject>
    {
        // 基底クラスのoldData/newDataを使用するため空実装
    };
    
    // ステージエディタマネージャー
    class StageEditorManager : public EditorManagerBase<StageEditorObject, StageEditorCommand>
    {
    private:
        using Base = EditorManagerBase<StageEditorObject, StageEditorCommand>;
        
        Vec2 m_playerSpawnPosition = Vec2(50, -5);
        Vec2 m_goalPosition = Vec2(20650, 0);
        Vec2 m_goalSize = Vec2(200, 200);
        
    public:
        // ===== ゴール・スポーン位置 =====
        
        Vec2 getPlayerSpawnPosition() const { return m_playerSpawnPosition; }
        void setPlayerSpawnPosition(const Vec2& pos) { m_playerSpawnPosition = pos; }
        
        Vec2 getGoalPosition() const { return m_goalPosition; }
        void setGoalPosition(const Vec2& pos) { m_goalPosition = pos; }
        
        Vec2 getGoalSize() const { return m_goalSize; }
        void setGoalSize(const Vec2& size) { m_goalSize = size; }
        
        // ===== オブジェクト操作 =====
        
        size_t addObject(const Stage::StageObject& obj) override;
        void removeObject(size_t index) override;
        void moveObject(size_t index, const Vec2& newPos);
        void modifyObject(size_t index, const Stage::StageObject& newObj);
        
        // ===== 選択オブジェクトの一括更新 =====
        
        void updateSelectedObjectsMovement(double distance, double speed, Stage::MovementType type);
        void updateSelectedObjectsDamage(double damage);
        
        // ===== 検索 =====
        
        Optional<size_t> findObjectAt(const Vec2& pos) const;
        bool hasObjectAtExactPosition(const RectF& rect) const;
        bool hasOverlappingObject(const RectF& rect) const;
        Optional<size_t> findGoalObject() const;
        void removeExistingGoal();
        
        // ===== ファイルI/O =====
        
        void saveToJSON(const FilePath& path) const override;
        void loadFromJSON(const FilePath& path) override;
        
    protected:
        // ===== コマンド実行 =====
        
        void executeUndoCommand(const StageEditorCommand& cmd) override;
        void executeRedoCommand(const StageEditorCommand& cmd) override;
        
    private:
        // ===== ユーティリティ =====
        
        Array<Stage::StageObject> mergeAdjacentObjects() const;
        JSON stageObjectToJSON(const Stage::StageObject& obj) const;
        Optional<Stage::StageObject> jsonToStageObject(const JSON& json) const;
    };
}