#pragma once
#include "../../../UseCase/Editor/EnemyEditor/EnemyEditorService.h"

namespace Jam::Presentation::Editor
{
    class EnemyEditorRenderer
    {
    private:
        UseCase::Editor::EnemyEditorService* m_service = nullptr;
        Font m_font{20};
        Font m_smallFont{16};
        
        mutable bool m_isEnemyTypeDropdownOpen = false;
        mutable bool m_switchToStageEditor = false;
        
        TextEditState m_patrolDistanceTextEdit;
        TextEditState m_patrolWaitTimeTextEdit;
        TextEditState m_foundDistanceTextEdit;
        TextEditState m_attackRangeTextEdit;
        TextEditState m_loseRangeTextEdit;
        TextEditState m_moveSpeedFactorTextEdit;
        
    public:
        void init(UseCase::Editor::EnemyEditorService* service);
        void drawEnemies() const;
        void drawGUIPanel() const;
        bool isEditorSwitchRequested() const { return m_switchToStageEditor; }
        void resetEditorSwitchRequest() const { m_switchToStageEditor = false; }
        
    private:
        void drawEnemy(const Domain::Editor::EnemyEditorObject& enemy, bool isSelected) const;
        void drawPatrolRange(const Domain::Editor::EnemyObject& enemy) const;
        String getEnemyTypeName(Domain::Editor::EnemyType type) const;
        ColorF getEnemyColor(Domain::Editor::EnemyType type) const;
    };
}
