#include <Siv3D.hpp>
#include "StageEditorRenderer.h"

namespace Jam::Presentation::Editor
{
    void StageEditorRenderer::init(const UseCase::Editor::StageEditorService* service)
    {
        m_service = service;
        m_editableService = const_cast<UseCase::Editor::StageEditorService*>(service);
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
        
        ColorF fillColor;
        if (isSelected)
        {
            fillColor = ColorF{1.0, 1.0, 0.0, 0.3};
        }
        else
        {
            switch (obj.stageObject.type)
            {
            case Domain::Stage::StageType::Normal:
                fillColor = ColorF{0.5, 0.8, 1.0, 0.2};
                break;
            case Domain::Stage::StageType::MovingPlatform:
                fillColor = ColorF{0.0, 0.8, 0.0, 0.3};
                break;
            case Domain::Stage::StageType::OneWayPlatform:
                fillColor = ColorF{0.3, 0.6, 1.0, 0.5};
                break;
            case Domain::Stage::StageType::DamagePlatform:
                fillColor = ColorF{1.0, 0.0, 0.0, 0.3};
                break;
            default:
                fillColor = ColorF{0.5, 0.8, 1.0, 0.2};
                break;
            }
        }
        
        ColorF frameColor = getGroundSideColor(obj.stageObject.groundSide);
        
        rect.draw(fillColor);
        
        switch (obj.stageObject.groundSide)
        {
        case Domain::Stage::GroundSide::All:
            break;
        case Domain::Stage::GroundSide::Up:
            Line{rect.tl(), rect.tr()}.draw(3.0, ColorF{0.0, 1.0, 0.0, 0.8});
            break;
        case Domain::Stage::GroundSide::Down:
            Line{rect.bl(), rect.br()}.draw(3.0, ColorF{0.0, 1.0, 0.0, 0.8});
            break;
        case Domain::Stage::GroundSide::Left:
            Line{rect.tl(), rect.bl()}.draw(3.0, ColorF{0.0, 1.0, 0.0, 0.8});
            break;
        case Domain::Stage::GroundSide::Right:
            Line{rect.tr(), rect.br()}.draw(3.0, ColorF{0.0, 1.0, 0.0, 0.8});
            break;
        case Domain::Stage::GroundSide::None:
            break;
        }
        
        if (isSelected)
        {
            rect.drawFrame(3.0, 0.0, Palette::Yellow);
        }
    }

    void StageEditorRenderer::drawGUIPanel()
    {
        if (!m_editableService) return;
        
        const int panelWidth = m_isPanelCollapsed ? 40 : 300;
        const int panelX = Scene::Width() - panelWidth;
        const int panelY = 0;
        const int panelHeight = Scene::Height();
        
        RectF{panelX, panelY, panelWidth, panelHeight}.draw(ColorF{0.15, 0.15, 0.15, 0.95});
        
        if (SimpleGUI::Button(m_isPanelCollapsed ? U"◀" : U"▶", Vec2{panelX + 5, 10}, 30))
        {
            m_isPanelCollapsed = !m_isPanelCollapsed;
        }
        
        if (m_isPanelCollapsed) return;
        
        int y = 50;
        
        m_font(U"ステージエディタ").draw(panelX + 10, y, Palette::White);
        y += 40;
        
        const String modeStr = [this]() {
            switch (m_editableService->getMode())
            {
            case Domain::Editor::StageEditorMode::Select: return U"選択";
            case Domain::Editor::StageEditorMode::Place: return U"配置";
            case Domain::Editor::StageEditorMode::Delete: return U"削除";
            default: return U"不明";
            }
        }();
        m_smallFont(U"モード: " + modeStr).draw(panelX + 10, y, Palette::Yellow);
        y += 35;
        
        SimpleGUI::Headline(U"ステージタイプ", Vec2{panelX + 10, y});
        y += 30;
        
        const String currentTypeName = getStageTypeName(m_editableService->getCurrentStageType());
        const RectF typeButtonRect{panelX + 10, y, 270, 30};
        
        if (typeButtonRect.leftClicked())
        {
            m_isStageTypeDropdownOpen = !m_isStageTypeDropdownOpen;
        }
        
        typeButtonRect.draw(m_isStageTypeDropdownOpen ? ColorF{0.3, 0.5, 0.7} : ColorF{0.2, 0.2, 0.2});
        typeButtonRect.drawFrame(1, Palette::White);
        m_smallFont(currentTypeName).draw(panelX + 20, y + 7, Palette::White);
        m_smallFont(m_isStageTypeDropdownOpen ? U"▲" : U"▼").draw(panelX + 250, y + 7, Palette::White);
        y += 35;
        
        if (m_isStageTypeDropdownOpen)
        {
            const Array<std::pair<String, Domain::Stage::StageType>> types = {
                {U"通常床", Domain::Stage::StageType::Normal},
                {U"動く床", Domain::Stage::StageType::MovingPlatform},
                {U"すり抜け床", Domain::Stage::StageType::OneWayPlatform},
                {U"ダメージ床", Domain::Stage::StageType::DamagePlatform}
            };
            
            for (const auto& [name, type] : types)
            {
                const RectF itemRect{panelX + 10, y, 270, 30};
                const bool isSelected = m_editableService->getCurrentStageType() == type;
                
                if (itemRect.leftClicked())
                {
                    m_editableService->setCurrentStageType(type);
                    m_isStageTypeDropdownOpen = false;
                }
                
                itemRect.draw(isSelected ? ColorF{0.3, 0.5, 0.7} : 
                             itemRect.mouseOver() ? ColorF{0.3, 0.3, 0.3} : ColorF{0.2, 0.2, 0.2});
                itemRect.drawFrame(1, Palette::White);
                m_smallFont(name).draw(panelX + 20, y + 7, Palette::White);
                y += 30;
            }
            y += 10;
        }
        
        SimpleGUI::Headline(U"接地面設定", Vec2{panelX + 10, y});
        y += 30;
        
        const Array<String> groundSideNames = {U"全方向", U"上", U"下", U"左", U"右", U"なし"};
        const String currentGroundSideName = groundSideNames[static_cast<size_t>(m_editableService->getCurrentGroundSide())];
        const RectF groundButtonRect{panelX + 10, y, 270, 30};
        
        if (groundButtonRect.leftClicked())
        {
            m_isGroundSideDropdownOpen = !m_isGroundSideDropdownOpen;
        }
        
        groundButtonRect.draw(m_isGroundSideDropdownOpen ? ColorF{0.3, 0.5, 0.7} : ColorF{0.2, 0.2, 0.2});
        groundButtonRect.drawFrame(1, Palette::White);
        m_smallFont(currentGroundSideName).draw(panelX + 20, y + 7, Palette::White);
        m_smallFont(m_isGroundSideDropdownOpen ? U"▲" : U"▼").draw(panelX + 250, y + 7, Palette::White);
        y += 35;
        
        if (m_isGroundSideDropdownOpen)
        {
            for (size_t i = 0; i < groundSideNames.size(); ++i)
            {
                const RectF itemRect{panelX + 10, y, 270, 30};
                const bool isSelected = static_cast<size_t>(m_editableService->getCurrentGroundSide()) == i;
                
                if (itemRect.leftClicked())
                {
                    m_editableService->setCurrentGroundSide(static_cast<Domain::Stage::GroundSide>(i));
                    m_isGroundSideDropdownOpen = false;
                }
                
                itemRect.draw(isSelected ? ColorF{0.3, 0.5, 0.7} : 
                             itemRect.mouseOver() ? ColorF{0.3, 0.3, 0.3} : ColorF{0.2, 0.2, 0.2});
                itemRect.drawFrame(1, Palette::White);
                m_smallFont(groundSideNames[i]).draw(panelX + 20, y + 7, Palette::White);
                y += 30;
            }
            y += 10;
        }
        
        if (m_editableService->getCurrentStageType() == Domain::Stage::StageType::MovingPlatform)
        {
            SimpleGUI::Headline(U"移動設定", Vec2{panelX + 10, y});
            y += 30;
            
            const Array<String> movementTypes = {U"横移動", U"縦移動", U"円運動"};
            const String currentMovementTypeName = movementTypes[static_cast<size_t>(m_editableService->getMovementType())];
            const RectF movementButtonRect{panelX + 10, y, 270, 30};
            
            if (movementButtonRect.leftClicked())
            {
                m_isMovementTypeDropdownOpen = !m_isMovementTypeDropdownOpen;
            }
            
            movementButtonRect.draw(m_isMovementTypeDropdownOpen ? ColorF{0.3, 0.5, 0.7} : ColorF{0.2, 0.2, 0.2});
            movementButtonRect.drawFrame(1, Palette::White);
            m_smallFont(currentMovementTypeName).draw(panelX + 20, y + 7, Palette::White);
            m_smallFont(m_isMovementTypeDropdownOpen ? U"▲" : U"▼").draw(panelX + 250, y + 7, Palette::White);
            y += 35;
            
            if (m_isMovementTypeDropdownOpen)
            {
                for (size_t i = 0; i < movementTypes.size(); ++i)
                {
                    const RectF itemRect{panelX + 10, y, 270, 30};
                    const bool isSelected = static_cast<size_t>(m_editableService->getMovementType()) == i;
                    
                    if (itemRect.leftClicked())
                    {
                        m_editableService->setMovementType(static_cast<Domain::Stage::MovementType>(i));
                        m_isMovementTypeDropdownOpen = false;
                    }
                    
                    itemRect.draw(isSelected ? ColorF{0.3, 0.5, 0.7} : 
                                 itemRect.mouseOver() ? ColorF{0.3, 0.3, 0.3} : ColorF{0.2, 0.2, 0.2});
                    itemRect.drawFrame(1, Palette::White);
                    m_smallFont(movementTypes[i]).draw(panelX + 20, y + 7, Palette::White);
                    y += 30;
                }
                y += 10;
            }
            
            double distance = m_editableService->getMovementDistance();
            if (SimpleGUI::Slider(U"距離: {:.0f}"_fmt(distance), distance, 50.0, 1000.0, Vec2{panelX + 10, y}, 140, 130))
            {
                m_editableService->setMovementDistance(distance);
            }
            y += 35;
            
            double speed = m_editableService->getMovementSpeed();
            if (SimpleGUI::Slider(U"速度: {:.0f}"_fmt(speed), speed, 10.0, 500.0, Vec2{panelX + 10, y}, 140, 130))
            {
                m_editableService->setMovementSpeed(speed);
            }
            y += 45;
        }
        
        const auto* selected = m_editableService->getStageManager().getSelectedObject();
        if (selected)
        {
            SimpleGUI::Headline(U"選択中", Vec2{panelX + 10, y});
            y += 25;
            m_smallFont(U"位置: ({:.0f}, {:.0f})"_fmt(selected->stageObject.rect.x, selected->stageObject.rect.y))
                .draw(panelX + 15, y, Palette::White);
            y += 20;
            m_smallFont(U"サイズ: ({:.0f}, {:.0f})"_fmt(selected->stageObject.rect.w, selected->stageObject.rect.h))
                .draw(panelX + 15, y, Palette::White);
            y += 30;
        }
        
        y = Scene::Height() - 150;
        SimpleGUI::Headline(U"操作", Vec2{panelX + 10, y});
        y += 25;
        m_smallFont(U"1: 選択モード").draw(panelX + 15, y, Palette::White);
        y += 20;
        m_smallFont(U"2: 配置モード").draw(panelX + 15, y, Palette::White);
        y += 20;
        m_smallFont(U"3: 削除モード").draw(panelX + 15, y, Palette::White);
        y += 25;
        m_smallFont(U"Ctrl+S: 保存").draw(panelX + 15, y, Palette::White);
        y += 20;
        m_smallFont(U"Ctrl+P: プレイ").draw(panelX + 15, y, Palette::White);
        y += 20;
        m_smallFont(U"ESC: 終了").draw(panelX + 15, y, Palette::White);
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
