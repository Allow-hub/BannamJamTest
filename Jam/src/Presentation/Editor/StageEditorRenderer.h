#pragma once
#include "../../UseCase/Editor/StageEditorService.h"

namespace Jam::Presentation::Editor
{
    class StageEditorRenderer
    {
    private:
        const UseCase::Editor::StageEditorService* m_service = nullptr;
        UseCase::Editor::StageEditorService* m_editableService = nullptr;
        Font m_font{20};
        Font m_smallFont{16};
        
        mutable bool m_isPanelCollapsed = false;
        mutable bool m_isStageTypeDropdownOpen = false;
        mutable bool m_isGroundSideDropdownOpen = false;
        mutable bool m_isMovementTypeDropdownOpen = false;
        
    public:
        void init(const UseCase::Editor::StageEditorService* service);
        void setEditableService(UseCase::Editor::StageEditorService* service) { m_editableService = service; }
        void draw() const;
        void drawGUIPanel();
        
    private:
        void drawGrid(const Camera2D& camera, int gridSize) const;
        void drawObjects(const Camera2D& camera, const Array<Domain::Editor::StageEditorObject>& objects) const;
        void drawObject(const Domain::Editor::StageEditorObject& obj, bool isSelected) const;
        void drawMovementGuide(const Domain::Stage::StageObject& obj) const;
        
        ColorF getGroundSideColor(Domain::Stage::GroundSide side) const;
        String getStageTypeName(Domain::Stage::StageType type) const;
    };
}