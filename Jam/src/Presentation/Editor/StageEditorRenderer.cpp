#include "StageEditorRenderer.h"

namespace Jam::Presentation::Editor
{
    void StageEditorRenderer::init(const UseCase::Editor::StageEditorService* service)
    {
        m_service = service;
    }

    void StageEditorRenderer::draw() const
    {
        if (!m_service) return;
        
        const auto& camera = m_service->getCamera();
        
        {
            auto transformer = camera.createTransformer();

            if (m_service->getSettings().isShowGrid())
            {
                drawGrid(camera, m_service->getSettings().getGridSize());
            }

            drawObjects(camera, m_service->getStageManager().getAllObjects());
        }

        drawUI();
    }

    void StageEditorRenderer::drawGrid(const Camera2D& camera, int gridSize) const
    {
        const int gridCount = 100;
        const ColorF gridColor{0.3, 0.3, 0.3, 0.5};
        
        for (int x = -gridCount; x <= gridCount; ++x)
        {
            Line{x * gridSize, -gridCount * gridSize, x * gridSize, gridCount * gridSize}
                .draw(0.5, gridColor);
        }
        
        for (int y = -gridCount; y <= gridCount; ++y)
        {
            Line{-gridCount * gridSize, y * gridSize, gridCount * gridSize, y * gridSize}
                .draw(0.5, gridColor);
        }
        
        Line{0, -gridCount * gridSize, 0, gridCount * gridSize}.draw(1.0, Palette::Red);
        Line{-gridCount * gridSize, 0, gridCount * gridSize, 0}.draw(1.0, Palette::Green);
    }

    void StageEditorRenderer::drawObjects(const Camera2D& camera, const Array<Domain::Editor::StageEditorObject>& objects) const
    {
        for (const auto& obj : objects)
        {
            drawObject(obj, obj.isSelected);
        }
    }

    void StageEditorRenderer::drawObject(const Domain::Editor::StageEditorObject& obj, bool isSelected) const
    {
        const auto& rect = obj.stageObject.rect;
        
        ColorF fillColor = isSelected ? ColorF{1.0, 1.0, 0.0, 0.3} : ColorF{0.5, 0.8, 1.0, 0.3};
        ColorF frameColor = getGroundSideColor(obj.stageObject.groundSide);
        
        rect.draw(fillColor);
        rect.drawFrame(2.0, frameColor);
        
        if (isSelected)
        {
            rect.drawFrame(4.0, Palette::Yellow);
        }
    }

    void StageEditorRenderer::drawUI() const
    {
        drawToolbar();
        drawObjectPalette();
        drawPropertyPanel();
        drawStatusBar();
    }

    void StageEditorRenderer::drawToolbar() const
    {
        const String modeStr = [this]() {
            switch (m_service->getMode())
            {
            case Domain::Editor::StageEditorMode::Select: return U"Select";
            case Domain::Editor::StageEditorMode::Place: return U"Place";
            case Domain::Editor::StageEditorMode::Delete: return U"Delete";
            case Domain::Editor::StageEditorMode::Move: return U"Move";
            case Domain::Editor::StageEditorMode::Test: return U"Test";
            default: return U"Unknown";
            }
        }();
        
        m_font(U"Mode: ", modeStr).draw(10, 10, Palette::White);
    }

    void StageEditorRenderer::drawObjectPalette() const
    {
        int y = 50;
        
        m_smallFont(U"StageType").draw(10, y, Palette::White);
        y += 30;
        
        m_smallFont(getStageTypeName(m_service->getCurrentStageType())).draw(10, y, Palette::Yellow);
        y += 30;
        
        m_smallFont(U"GroundSide").draw(10, y, Palette::White);
        y += 30;
        
        const String groundSideStr = [this]() {
            switch (m_service->getCurrentGroundSide())
            {
            case Domain::Stage::GroundSide::All: return U"All";
            case Domain::Stage::GroundSide::Up: return U"Up";
            case Domain::Stage::GroundSide::Down: return U"Down";
            case Domain::Stage::GroundSide::Left: return U"Left";
            case Domain::Stage::GroundSide::Right: return U"Right";
            case Domain::Stage::GroundSide::None: return U"None";
            default: return U"Unknown";
            }
        }();
        
        m_smallFont(groundSideStr).draw(10, y, Palette::Yellow);
    }

    void StageEditorRenderer::drawPropertyPanel() const
    {
        const auto* selected = m_service->getStageManager().getSelectedObject();
        if (!selected) return;
        
        int x = Scene::Width() - 200;
        int y = 10;
        
        m_smallFont(U"Selected Object").draw(x, y, Palette::White);
        y += 30;
        
        m_smallFont(U"Pos: ({:.0f}, {:.0f})"_fmt(selected->stageObject.rect.x, selected->stageObject.rect.y))
            .draw(x, y, Palette::White);
        y += 25;
        
        m_smallFont(U"Size: ({:.0f}, {:.0f})"_fmt(selected->stageObject.rect.w, selected->stageObject.rect.h))
            .draw(x, y, Palette::White);
    }

    void StageEditorRenderer::drawStatusBar() const
    {
        int y = Scene::Height() - 30;
        
        String help = U"1:Select 2:Place 3:Delete | LClick:Action RClick:Move | Ctrl+S:Save Ctrl+Z:Undo";
        m_smallFont(help).draw(10, y, Palette::White);
    }

    ColorF StageEditorRenderer::getGroundSideColor(Domain::Stage::GroundSide side) const
    {
        switch (side)
        {
        case Domain::Stage::GroundSide::All: return Palette::White;
        case Domain::Stage::GroundSide::Up: return Palette::Red;
        case Domain::Stage::GroundSide::Down: return Palette::Blue;
        case Domain::Stage::GroundSide::Left: return Palette::Green;
        case Domain::Stage::GroundSide::Right: return Palette::Yellow;
        case Domain::Stage::GroundSide::None: return Palette::Gray;
        default: return Palette::White;
        }
    }

    String StageEditorRenderer::getStageTypeName(Domain::Stage::StageType type) const
    {
        switch (type)
        {
        case Domain::Stage::StageType::Normal: return U"Normal";
        case Domain::Stage::StageType::MovingPlatform: return U"Moving";
        case Domain::Stage::StageType::DamagePlatform: return U"Damage";
        default: return U"Unknown";
        }
    }
}
