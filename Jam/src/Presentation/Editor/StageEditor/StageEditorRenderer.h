#pragma once
#include "../../../UseCase/Editor/StageEditor/StageEditorService.h"

namespace Jam::Presentation::Editor
{
    class StageEditorRenderer
    {
    private:
        UseCase::Editor::StageEditorService* m_service = nullptr;
        Font m_font{20};
        Font m_smallFont{16};
        
        mutable bool m_isPanelCollapsed = false;
        mutable bool m_isStageTypeDropdownOpen = false;
        mutable bool m_isMovementTypeDropdownOpen = false;
        mutable bool m_switchToEnemyEditor = false;
        
        mutable TextEditState m_metadataTextEdit;
        mutable TextEditState m_distanceTextEdit;
        mutable TextEditState m_speedTextEdit;
        mutable TextEditState m_damageTextEdit;
        
    public:
        void init(UseCase::Editor::StageEditorService* service);
        void draw() const;
        void drawGUIPanel() const;
        bool isEditorSwitchRequested() const { return m_switchToEnemyEditor; }
        void resetEditorSwitchRequest() const { m_switchToEnemyEditor = false; }
        
    private:
        void drawGrid(const Camera2D& camera, int gridSize) const;
        void drawObjects(const Camera2D& camera, const Array<Domain::Editor::StageEditorObject>& objects) const;
        void drawObject(const Domain::Editor::StageEditorObject& obj, bool isSelected) const;
        void drawMovementGuide(const Domain::Stage::StageObject& obj) const;
        
        ColorF getGroundSideColor(Domain::Stage::GroundSide side) const;
        String getStageTypeName(Domain::Stage::StageType type) const;
    };
}