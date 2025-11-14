#pragma once
#include "../../UseCase/Editor/StageEditorService.h"

namespace Jam::Presentation::Editor
{
    class StageEditorRenderer
    {
    private:
        UseCase::Editor::StageEditorService* m_service = nullptr;
        Font m_font{20};
        Font m_smallFont{16};
        
        bool m_isPanelCollapsed = false;
        bool m_isStageTypeDropdownOpen = false;
        bool m_isMovementTypeDropdownOpen = false;
        
        TextEditState m_metadataTextEdit;
        TextEditState m_distanceTextEdit;
        TextEditState m_speedTextEdit;
        TextEditState m_damageTextEdit;
        
    public:
        void init(UseCase::Editor::StageEditorService* service);
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