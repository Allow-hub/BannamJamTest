#pragma once
#include "../../UseCase/Editor/StageEditorService.h"

namespace Jam::Presentation::Editor
{
    class StageEditorRenderer
    {
    private:
        const UseCase::Editor::StageEditorService* m_service = nullptr;
        Font m_font{20};
        Font m_smallFont{16};
        
    public:
        void init(const UseCase::Editor::StageEditorService* service);
        void draw() const;
        
    private:
        void drawGrid(const Camera2D& camera, int gridSize) const;
        void drawObjects(const Camera2D& camera, const Array<Domain::Editor::StageEditorObject>& objects) const;
        void drawObject(const Domain::Editor::StageEditorObject& obj, bool isSelected) const;
        void drawUI() const;
        void drawToolbar() const;
        void drawObjectPalette() const;
        void drawPropertyPanel() const;
        void drawStatusBar() const;
        
        ColorF getGroundSideColor(Domain::Stage::GroundSide side) const;
        String getStageTypeName(Domain::Stage::StageType type) const;
    };
}