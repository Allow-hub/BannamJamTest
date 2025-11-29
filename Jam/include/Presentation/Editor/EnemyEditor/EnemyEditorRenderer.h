#pragma once
#include "../Base/EditorRendererBase.h"
#include "../../../UseCase/Editor/EnemyEditor/EnemyEditorService.h"
#include "../../../Domain/Editor/StageEditor/StageEditorManager.h"
#include "../Utilities/EditorGridUtil.h"

namespace Jam::Presentation::Editor
{
    // 敵エディタレンダラー
    class EnemyEditorRenderer : public EditorRendererBase<UseCase::Editor::EnemyEditorService>
    {
    private:
        static constexpr double PLAYER_SPAWN_RADIUS = 20.0;
        static constexpr double PLAYER_SPAWN_CENTER_RADIUS = 5.0;
        static constexpr double FRAME_THICKNESS = 2.0;
        
        mutable bool m_isEnemyTypeDropdownOpen = false;
        mutable bool m_switchToStageEditor = false;
        const Domain::Editor::StageEditorManager* m_stageManager = nullptr;
        
        // テキスト入力フィールド
        mutable TextEditState m_patrolDistanceTextEdit;
        mutable TextEditState m_waitTimeTextEdit;
        mutable TextEditState m_foundDistanceTextEdit;
        mutable TextEditState m_attackRangeTextEdit;
        mutable TextEditState m_loseRangeTextEdit;
        mutable TextEditState m_speedFactorTextEdit;
        
    public:
        // StageEditorManagerへの参照を設定
        void setStageManager(const Domain::Editor::StageEditorManager* manager);
        
        void drawView() const override;
        
        void drawGUIPanel() const override;
        
        bool isEditorSwitchRequested() const { return m_switchToStageEditor; }
        void resetEditorSwitchRequest() const { m_switchToStageEditor = false; }
        
        // テキスト入力中かどうかを判定
        bool isTextInputActive() const;
        
        // StageEditorSceneから呼び出すためpublicに
        void drawEnemy(const Domain::Editor::EnemyEditorObject& enemy, bool isSelected) const;
        void drawPatrolRange(const Domain::Editor::EnemyObject& enemy) const;
        
    private:
        int drawCurrentMode(int y) const;
        int drawEnemyTypeSelector(int y) const;
        int drawPatrolSettings(int y) const;
        int drawChaseSettings(int y) const;
        String getEnemyTypeName(Domain::EnemyType type) const;
        ColorF getEnemyColor(Domain::EnemyType type) const;
    };
}