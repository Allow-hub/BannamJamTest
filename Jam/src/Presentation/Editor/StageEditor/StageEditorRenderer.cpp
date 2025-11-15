#include <Siv3D.hpp>
#include "StageEditorRenderer.h"

namespace Jam::Presentation::Editor
{
    void StageEditorRenderer::init(UseCase::Editor::StageEditorService* service)
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
            
            if (auto dragRect = m_service->getDragRect())
            {
                dragRect->draw(ColorF{0.0, 1.0, 0.0, 0.3});
                
                const double x = dragRect->x;
                const double y = dragRect->y;
                const double w = dragRect->w;
                const double h = dragRect->h;
                const double lineWidth = 2.0;
                const ColorF lineColor{0.0, 1.0, 0.0, 0.8};
                
                Line{x, y, x + w, y}.draw(lineWidth, lineColor);
                Line{x, y + h, x + w, y + h}.draw(lineWidth, lineColor);
                Line{x, y, x, y + h}.draw(lineWidth, lineColor);
                Line{x + w, y, x + w, y + h}.draw(lineWidth, lineColor);
            }
        }
        
        drawGUIPanel();
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
            Line{xPos, startY * gridSize, xPos, endY * gridSize}.draw(0.5, gridColor);
        }
        
        for (int y = startY; y <= endY; ++y)
        {
            double yPos = y * gridSize;
            Line{startX * gridSize, yPos, endX * gridSize, yPos}.draw(0.5, gridColor);
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
        
        const Vec2 playerSpawnPos{0, 0};
        const double playerRadius = 20.0;
        Circle{playerSpawnPos, playerRadius}.draw(ColorF{0.0, 1.0, 1.0, 0.5});
        Circle{playerSpawnPos, playerRadius}.drawFrame(2.0, ColorF{0.0, 1.0, 1.0});
        Circle{playerSpawnPos, 5.0}.draw(ColorF{1.0, 1.0, 1.0});
        
        for (const auto& obj : objects)
        {
            if (viewRect.intersects(obj.stageObject.rect))
            {
                drawObject(obj, obj.isSelected);
                
                if (obj.stageObject.type == Domain::Stage::StageType::MovingPlatform)
                {
                    drawMovementGuide(obj.stageObject);
                }
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
        
        rect.draw(fillColor);
        
        switch (obj.stageObject.groundSide)
        {
        case Domain::Stage::GroundSide::All:
            Line{rect.tl(), rect.tr()}.stretched(2.0, 0).draw(4.0, ColorF{0.0, 1.0, 0.0});
            Line{rect.bl(), rect.br()}.stretched(2.0, 0).draw(4.0, ColorF{0.0, 1.0, 0.0});
            break;
        case Domain::Stage::GroundSide::Up:
            Line{rect.tl(), rect.tr()}.stretched(2.0, 0).draw(4.0, ColorF{0.0, 1.0, 0.0});
            break;
        case Domain::Stage::GroundSide::Down:
            Line{rect.bl(), rect.br()}.stretched(2.0, 0).draw(4.0, ColorF{0.0, 1.0, 0.0});
            break;
        case Domain::Stage::GroundSide::Left:
            Line{rect.tl(), rect.bl()}.stretched(0, 2.0).draw(4.0, ColorF{0.0, 1.0, 0.0});
            break;
        case Domain::Stage::GroundSide::Right:
            Line{rect.tr(), rect.br()}.stretched(0, 2.0).draw(4.0, ColorF{0.0, 1.0, 0.0});
            break;
        case Domain::Stage::GroundSide::None:
            break;
        }
        
        if (isSelected)
        {
            rect.drawFrame(3.0, 0.0, Palette::Yellow);
        }
    }

    void StageEditorRenderer::drawGUIPanel() const
    {
        if (!m_service) return;
        
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
        
        if (SimpleGUI::Button(U"Switch to Enemy Editor", Vec2{panelX + 10, y}, 280))
        {
            m_switchToEnemyEditor = true;
        }
        y += 45;
        
        SimpleGUI::Headline(U"カメラ設定", Vec2{panelX + 10, y});
        y += 40;
        
        double cameraSpeed = m_service->getSettings().getCameraSpeed();
        if (SimpleGUI::Slider(U"移動速度: {:.1f}"_fmt(cameraSpeed), cameraSpeed, 1.0, 30.0, Vec2{panelX + 10, y}, 120, 120))
        {
            m_service->getSettings().setCameraSpeed(cameraSpeed);
        }
        y += 45;
        
        const String modeStr = [this]() {
            switch (m_service->getMode())
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
        
        const String currentTypeName = getStageTypeName(m_service->getCurrentStageType());
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
                const bool isSelected = m_service->getCurrentStageType() == type;
                
                if (itemRect.leftClicked())
                {
                    m_service->setCurrentStageType(type);
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
        y += 40;
        
        const auto currentType = m_service->getCurrentStageType();
        
        String groundSideText;
        if (currentType == Domain::Stage::StageType::Normal || 
            currentType == Domain::Stage::StageType::MovingPlatform)
        {
            groundSideText = U"上下の接地判定";
        }
        else if (currentType == Domain::Stage::StageType::OneWayPlatform)
        {
            groundSideText = U"上のみの接地判定";
        }
        else if (currentType == Domain::Stage::StageType::DamagePlatform || 
                 currentType == Domain::Stage::StageType::MovingDamagePlatform)
        {
            groundSideText = U"接地判定なし";
        }
        
        m_smallFont(groundSideText).draw(panelX + 15, y, ColorF{0.0, 1.0, 0.0});
        y += 40;
        
        y += 15;
        
        if (m_service->getCurrentStageType() == Domain::Stage::StageType::MovingPlatform)
        {
            SimpleGUI::Headline(U"移動設定", Vec2{panelX + 10, y});
            y += 40;
            
            const Array<String> movementTypes = {U"横移動", U"縦移動", U"円運動"};
            const String currentMovementTypeName = movementTypes[static_cast<size_t>(m_service->getMovementType())];
            const RectF movementButtonRect{panelX + 10, y, 270, 30};
            
            if (movementButtonRect.leftClicked())
            {
                m_isMovementTypeDropdownOpen = !m_isMovementTypeDropdownOpen;
            }
            
            movementButtonRect.draw(m_isMovementTypeDropdownOpen ? ColorF{0.3, 0.5, 0.7} : ColorF{0.2, 0.2, 0.2});
            movementButtonRect.drawFrame(1, Palette::White);
            m_smallFont(currentMovementTypeName).draw(panelX + 20, y + 7, Palette::White);
            m_smallFont(m_isMovementTypeDropdownOpen ? U"▲" : U"▼").draw(panelX + 250, y + 7, Palette::White);
            y += 40;
            
            if (m_isMovementTypeDropdownOpen)
            {
                for (size_t i = 0; i < movementTypes.size(); ++i)
                {
                    const RectF itemRect{panelX + 10, y, 270, 30};
                    const bool isSelected = static_cast<size_t>(m_service->getMovementType()) == i;
                    
                    if (itemRect.leftClicked())
                    {
                        m_service->setMovementType(static_cast<Domain::Stage::MovementType>(i));
                        m_isMovementTypeDropdownOpen = false;
                    }
                    
                    itemRect.draw(isSelected ? ColorF{0.3, 0.5, 0.7} : 
                                 itemRect.mouseOver() ? ColorF{0.3, 0.3, 0.3} : ColorF{0.2, 0.2, 0.2});
                    itemRect.drawFrame(1, Palette::White);
                    m_smallFont(movementTypes[i]).draw(panelX + 20, y + 7, Palette::White);
                    y += 30;
                }
                y += 15;
            }
            
            const bool isCircular = m_service->getMovementType() == Domain::Stage::MovementType::Circular;
            const String distanceLabel = isCircular ? U"半径" : U"距離";
            
            double distance = m_service->getMovementDistance();
            if (SimpleGUI::Slider(distanceLabel + U": {:.0f}"_fmt(distance), distance, 50.0, 1000.0, Vec2{panelX + 10, y}, 100, 100))
            {
                m_service->setMovementDistance(distance);
            }
            y += 30;
            
            String distanceStr = U"{:.0f}"_fmt(distance);
            if (m_distanceTextEdit.text != distanceStr && !m_distanceTextEdit.active)
            {
                m_distanceTextEdit.text = distanceStr;
            }
            if (SimpleGUI::TextBox(m_distanceTextEdit, Vec2{panelX + 210, y - 25}, 70))
            {
                if (auto value = ParseOpt<double>(m_distanceTextEdit.text))
                {
                    m_service->setMovementDistance(Clamp(*value, 50.0, 1000.0));
                }
            }
            y += 20;
            
            double speed = m_service->getMovementSpeed();
            if (SimpleGUI::Slider(U"速度: {:.0f}"_fmt(speed), speed, 10.0, 500.0, Vec2{panelX + 10, y}, 100, 100))
            {
                m_service->setMovementSpeed(speed);
            }
            y += 30;
            
            String speedStr = U"{:.0f}"_fmt(speed);
            if (m_speedTextEdit.text != speedStr && !m_speedTextEdit.active)
            {
                m_speedTextEdit.text = speedStr;
            }
            if (SimpleGUI::TextBox(m_speedTextEdit, Vec2{panelX + 210, y - 25}, 70))
            {
                if (auto value = ParseOpt<double>(m_speedTextEdit.text))
                {
                    m_service->setMovementSpeed(Clamp(*value, 10.0, 500.0));
                }
            }
            y += 25;
            
            bool loopMovement = m_service->getLoopMovement();
            if (SimpleGUI::CheckBox(loopMovement, U"ループ移動", Vec2{panelX + 10, y}, 270))
            {
                m_service->setLoopMovement(loopMovement);
            }
            y += 45;
        }
        
        if (m_service->getCurrentStageType() == Domain::Stage::StageType::DamagePlatform ||
            m_service->getCurrentStageType() == Domain::Stage::StageType::MovingDamagePlatform)
        {
            SimpleGUI::Headline(U"ダメージ設定", Vec2{panelX + 10, y});
            y += 40;
            
            double damageAmount = m_service->getDamageAmount();
            if (SimpleGUI::Slider(U"ダメージ: {:.0f}"_fmt(damageAmount), damageAmount, 1.0, 50.0, Vec2{panelX + 10, y}, 100, 100))
            {
                m_service->setDamageAmount(Math::Round(damageAmount));
            }
            y += 30;
            
            String damageStr = U"{:.0f}"_fmt(damageAmount);
            if (m_damageTextEdit.text != damageStr && !m_damageTextEdit.active)
            {
                m_damageTextEdit.text = damageStr;
            }
            if (SimpleGUI::TextBox(m_damageTextEdit, Vec2{panelX + 210, y - 25}, 70))
            {
                if (auto value = ParseOpt<int>(m_damageTextEdit.text))
                {
                    m_service->setDamageAmount(Clamp(*value, 1, 50));
                }
            }
            y += 35;
        }
        
        SimpleGUI::Headline(U"識別名", Vec2{panelX + 10, y});
        y += 40;
        
        if (m_metadataTextEdit.text != m_service->getMetadata())
        {
            m_metadataTextEdit.text = m_service->getMetadata();
        }
        
        if (SimpleGUI::TextBox(m_metadataTextEdit, Vec2{panelX + 10, y}, 270, 20))
        {
            m_service->setMetadata(m_metadataTextEdit.text);
        }
        y += 35;
        m_smallFont(U"(オブジェクトの識別用名称)").draw(panelX + 15, y, ColorF{0.7, 0.7, 0.7});
        y += 45;
        
        const Array<const Domain::Editor::StageEditorObject*> selectedObjects = m_service->getStageManager().getSelectedObjects();
        if (!selectedObjects.isEmpty())
        {
            SimpleGUI::Headline(U"選択中", Vec2{panelX + 10, y});
            y += 25;
            
            if (selectedObjects.size() == 1)
            {
                const auto* selected = selectedObjects[0];
                m_smallFont(U"位置: ({:.0f}, {:.0f})"_fmt(selected->stageObject.rect.x, selected->stageObject.rect.y))
                    .draw(panelX + 15, y, Palette::White);
                y += 20;
                m_smallFont(U"サイズ: ({:.0f}, {:.0f})"_fmt(selected->stageObject.rect.w, selected->stageObject.rect.h))
                    .draw(panelX + 15, y, Palette::White);
                y += 30;
            }
            else
            {
                m_smallFont(U"{} 個のオブジェクトを選択中"_fmt(selectedObjects.size()))
                    .draw(panelX + 15, y, Palette::White);
                y += 30;
            }
        }
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

    void StageEditorRenderer::drawMovementGuide(const Domain::Stage::StageObject& obj) const
    {
        const Vec2 center = obj.rect.center();
        const double distance = obj.movementDistance;
        const ColorF guideColor{1.0, 0.5, 0.0, 0.8};
        const double arrowSize = 15.0;
        
        Vec2 startPos, endPos;
        
        switch (obj.movementType)
        {
        case Domain::Stage::MovementType::Horizontal:
            startPos = Vec2{center.x - distance / 2, center.y};
            endPos = Vec2{center.x + distance / 2, center.y};
            
            Line{startPos, endPos}.draw(3.0, guideColor);
            
            Triangle{
                Vec2{endPos.x, endPos.y},
                Vec2{endPos.x - arrowSize, endPos.y - arrowSize / 2},
                Vec2{endPos.x - arrowSize, endPos.y + arrowSize / 2}
            }.draw(guideColor);
            
            Triangle{
                Vec2{startPos.x, startPos.y},
                Vec2{startPos.x + arrowSize, startPos.y - arrowSize / 2},
                Vec2{startPos.x + arrowSize, startPos.y + arrowSize / 2}
            }.draw(guideColor);
            
            Circle{startPos, 5.0}.draw(guideColor);
            Circle{endPos, 5.0}.draw(guideColor);
            break;
            
        case Domain::Stage::MovementType::Vertical:
            startPos = Vec2{center.x, center.y - distance / 2};
            endPos = Vec2{center.x, center.y + distance / 2};
            
            Line{startPos, endPos}.draw(3.0, guideColor);
            
            Triangle{
                Vec2{endPos.x, endPos.y},
                Vec2{endPos.x - arrowSize / 2, endPos.y - arrowSize},
                Vec2{endPos.x + arrowSize / 2, endPos.y - arrowSize}
            }.draw(guideColor);
            
            Triangle{
                Vec2{startPos.x, startPos.y},
                Vec2{startPos.x - arrowSize / 2, startPos.y + arrowSize},
                Vec2{startPos.x + arrowSize / 2, startPos.y + arrowSize}
            }.draw(guideColor);
            
            Circle{startPos, 5.0}.draw(guideColor);
            Circle{endPos, 5.0}.draw(guideColor);
            break;
            
        case Domain::Stage::MovementType::Circular:
            Circle{center, distance}.drawFrame(3.0, guideColor);
            break;
        }
    }

    String StageEditorRenderer::getStageTypeName(Domain::Stage::StageType type) const
    {
        switch (type)
        {
        case Domain::Stage::StageType::Normal: return U"通常";
        case Domain::Stage::StageType::MovingPlatform: return U"動く床";
        case Domain::Stage::StageType::OneWayPlatform: return U"すり抜け床";
        case Domain::Stage::StageType::DamagePlatform: return U"ダメージ床";
        default: return U"不明";
        }
    }
}