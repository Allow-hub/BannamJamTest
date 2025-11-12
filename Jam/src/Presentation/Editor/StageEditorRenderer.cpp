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
        const Vec2 center = camera.getCenter();
        const double scale = camera.getScale();
        const double viewWidth = Scene::Width() / scale;
        const double viewHeight = Scene::Height() / scale;
        
        const int startX = static_cast<int>((center.x - viewWidth / 2) / gridSize) - 1;
        const int endX = static_cast<int>((center.x + viewWidth / 2) / gridSize) + 1;
        const int startY = static_cast<int>((center.y - viewHeight / 2) / gridSize) - 1;
        const int endY = static_cast<int>((center.y + viewHeight / 2) / gridSize) + 1;
        
        const ColorF gridColor{0.3, 0.3, 0.3, 0.5};
        
        for (int x = startX; x <= endX; ++x)
        {
            double xPos = x * gridSize;
            Line{xPos, startY * gridSize, xPos, endY * gridSize}
                .draw(0.5, gridColor);
        }
        
        for (int y = startY; y <= endY; ++y)
        {
            double yPos = y * gridSize;
            Line{startX * gridSize, yPos, endX * gridSize, yPos}
                .draw(0.5, gridColor);
        }
        
        Line{0, startY * gridSize, 0, endY * gridSize}.draw(2.0, Palette::Red);
        Line{startX * gridSize, 0, endX * gridSize, 0}.draw(2.0, Palette::Green);
    }

    void StageEditorRenderer::drawObjects(const Camera2D& camera, const Array<Domain::Editor::StageEditorObject>& objects) const
    {
        const Vec2 center = camera.getCenter();
        const double scale = camera.getScale();
        const double viewWidth = Scene::Width() / scale;
        const double viewHeight = Scene::Height() / scale;
        const RectF viewRect{center.x - viewWidth / 2, center.y - viewHeight / 2, viewWidth, viewHeight};
        
        for (const auto& obj : objects)
        {
            if (viewRect.intersects(obj.stageObject.rect))
            {
                drawObject(obj, obj.isSelected);
            }
        }
    }

    void StageEditorRenderer::drawObject(const Domain::Editor::StageEditorObject& obj, bool isSelected) const
    {
        const auto& rect = obj.stageObject.rect;
        
        ColorF fillColor = isSelected ? ColorF{1.0, 1.0, 0.0, 0.3} : ColorF{0.5, 0.8, 1.0, 0.2};
        ColorF frameColor = getGroundSideColor(obj.stageObject.groundSide);
        
        rect.draw(fillColor).drawFrame(2.0, frameColor);
        
        if (isSelected)
        {
            rect.drawFrame(3.0, 0.0, Palette::Yellow);
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
            case Domain::Editor::StageEditorMode::Select: return U"選択";
            case Domain::Editor::StageEditorMode::Place: return U"配置";
            case Domain::Editor::StageEditorMode::Delete: return U"削除";
            case Domain::Editor::StageEditorMode::Move: return U"移動";
            case Domain::Editor::StageEditorMode::Test: return U"テスト";
            default: return U"不明";
            }
        }();
        
        m_font(U"モード: ", modeStr).draw(10, 10, Palette::White);
    }

    void StageEditorRenderer::drawObjectPalette() const
    {
        int y = 50;
        
        m_smallFont(U"ステージタイプ").draw(10, y, Palette::White);
        y += 30;
        
        m_smallFont(getStageTypeName(m_service->getCurrentStageType())).draw(10, y, Palette::Yellow);
        y += 30;
        
        m_smallFont(U"接地面").draw(10, y, Palette::White);
        y += 30;
        
        const String groundSideStr = [this]() {
            switch (m_service->getCurrentGroundSide())
            {
            case Domain::Stage::GroundSide::All: return U"全方向";
            case Domain::Stage::GroundSide::Up: return U"上";
            case Domain::Stage::GroundSide::Down: return U"下";
            case Domain::Stage::GroundSide::Left: return U"左";
            case Domain::Stage::GroundSide::Right: return U"右";
            case Domain::Stage::GroundSide::None: return U"なし";
            default: return U"不明";
            }
        }();
        
        ColorF sideColor = getGroundSideColor(m_service->getCurrentGroundSide());
        m_smallFont(groundSideStr).draw(10, y, sideColor);
        
        y += 30;
        
        const String movementTypeStr = [this]() {
            switch (m_service->getMovementType())
            {
            case Domain::Stage::MovementType::Horizontal: return U"横移動";
            case Domain::Stage::MovementType::Vertical: return U"縦移動";
            case Domain::Stage::MovementType::Circular: return U"円運動";
            default: return U"不明";
            }
        }();
        m_smallFont(U"移動タイプ: {}"_fmt(movementTypeStr)).draw(10, y, Palette::Orange);
        
        y += 25;
        m_smallFont(U"移動距離: {:.0f}"_fmt(m_service->getMovementDistance())).draw(10, y, Palette::Orange);
        
        y += 25;
        m_smallFont(U"移動速度: {:.0f}"_fmt(m_service->getMovementSpeed())).draw(10, y, Palette::Orange);
        
        y += 30;
        m_smallFont(U"配置方向").draw(10, y, Palette::White);
        y += 25;
        
        const String orientationStr = (m_service->getPlacementOrientation() == Domain::Editor::PlacementOrientation::Horizontal) 
            ? U"水平 (Shift)" : U"垂直 (Alt)";
        m_smallFont(orientationStr).draw(10, y, Palette::Cyan);
        
        y += 40;
        m_smallFont(U"枠線の色:").draw(10, y, Palette::White);
        y += 25;
        m_smallFont(U"白:全方向").draw(15, y, Palette::White); y += 20;
        m_smallFont(U"赤:上").draw(15, y, Palette::Red); y += 20;
        m_smallFont(U"青:下").draw(15, y, Palette::Blue); y += 20;
        m_smallFont(U"緑:左").draw(15, y, Palette::Green); y += 20;
        m_smallFont(U"黄:右").draw(15, y, Palette::Yellow);
    }

    void StageEditorRenderer::drawPropertyPanel() const
    {
        const auto* selected = m_service->getStageManager().getSelectedObject();
        if (!selected) return;
        
        int x = Scene::Width() - 200;
        int y = 10;
        
        m_smallFont(U"選択中のオブジェクト").draw(x, y, Palette::White);
        y += 30;
        
        m_smallFont(U"位置: ({:.0f}, {:.0f})"_fmt(selected->stageObject.rect.x, selected->stageObject.rect.y))
            .draw(x, y, Palette::White);
        y += 25;
        
        m_smallFont(U"サイズ: ({:.0f}, {:.0f})"_fmt(selected->stageObject.rect.w, selected->stageObject.rect.h))
            .draw(x, y, Palette::White);
    }

    void StageEditorRenderer::drawStatusBar() const
    {
        int y = Scene::Height() - 30;
        
        String help = U"1:選択 2:配置 3:削除 | Q:タイプ E:接地面 Z:移動方向 X/Shift+X:距離 C/Shift+C:速度 | Ctrl+S:保存 Ctrl+P:プレイ | ESC:終了";
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
        case Domain::Stage::StageType::Normal: return U"通常";
        case Domain::Stage::StageType::MovingPlatform: return U"動く床";
        case Domain::Stage::StageType::DamagePlatform: return U"ダメージ床";
        default: return U"不明";
        }
    }
}
