// ========================================
// StageEditorRenderer.h（リファクタリング版）
// ========================================
#pragma once
#include "../Base/EditorRendererBase.h"
#include "../../../UseCase/Editor/StageEditor/StageEditorService.h"

namespace Jam::Presentation::Editor
{
    class StageEditorRenderer : public EditorRendererBase<UseCase::Editor::StageEditorService>
    {
    private:
        mutable bool m_isStageTypeDropdownOpen = false;
        mutable bool m_isMovementTypeDropdownOpen = false;
        mutable bool m_switchToEnemyEditor = false;
        
        mutable TextEditState m_metadataTextEdit;
        mutable TextEditState m_distanceTextEdit;
        mutable TextEditState m_speedTextEdit;
        mutable TextEditState m_damageTextEdit;
        
    public:
        void drawView() const override
        {
            const auto& camera = this->m_service->getCamera();
            auto transformer = camera.createTransformer();
            
            // グリッドは常に表示
            drawGridForStage(camera);
            
            drawObjects(camera, this->m_service->getManager().getAllObjects());
            
            if (auto dragRect = this->m_service->getDragRect())
            {
                dragRect->draw(ColorF{0.0, 1.0, 0.0, 0.3});
                dragRect->drawFrame(2.0, ColorF{0.0, 1.0, 0.0, 0.8});
            }
        }
        
        void drawGUIPanel() const override
        {
            this->handlePanelScroll();
            this->drawBasicPanel();
            
            // 折りたたみボタンとタイトル（スクロール対象外）
            this->drawTitleAndToggle(U"ステージエディタ");
            
            if (this->m_isPanelCollapsed) return;
            
            {
                auto transformer = this->createPanelTransformer();
                
                int y = this->getContentStartY();
                
                if (SimpleGUI::Button(U"敵エディタに切り替え", Vec2{this->getPanelX() + 10, y}, 280))
                {
                    m_switchToEnemyEditor = true;
                }
                y += 45;
                
                y = this->drawCameraSettings(y);
                y = this->drawModeDisplay(y);
                y = drawCurrentMode(y);
                y = drawStageTypeSelector(y);
                y = drawGroundSideInfo(y);
                y = drawMovementSettings(y);
                y = drawDamageSettings(y);
                y = drawMetadataEdit(y);
                y = drawSelectionInfo(y);
            }
        }
        
        bool isEditorSwitchRequested() const { return m_switchToEnemyEditor; }
        void resetEditorSwitchRequest() const { m_switchToEnemyEditor = false; }
        
        // StageEditorSceneから呼び出すためpublicに
        void drawObject(const Domain::Editor::StageEditorObjectNew& obj, bool isSelected) const;
        
    private:
        int drawCurrentMode(int y) const
        {
            SimpleGUI::Headline(U"現在のモード", Vec2{this->getPanelX() + 10, y});
            y += 35;
            
            String modeName;
            ColorF modeColor;
            switch (this->m_service->getMode())
            {
            case UseCase::Editor::EditorMode::Select:
                modeName = U"選択モード";
                modeColor = Palette::Cyan;
                break;
            case UseCase::Editor::EditorMode::Place:
                modeName = U"配置モード";
                modeColor = Palette::Lime;
                break;
            case UseCase::Editor::EditorMode::Delete:
                modeName = U"削除モード";
                modeColor = Palette::Red;
                break;
            }
            
            this->m_font(modeName).draw(this->getPanelX() + 15, y, modeColor);
            y += 45;
            
            return y;
        }
        
        int drawStageTypeSelector(int y) const
        {
            SimpleGUI::Headline(U"ステージタイプ", Vec2{this->getPanelX() + 10, y});
            y += 30;
            
            const String currentTypeName = getStageTypeName(this->m_service->getCurrentStageType());
            if (SimpleGUI::Button(currentTypeName, Vec2{this->getPanelX() + 10, y}, 270))
            {
                m_isStageTypeDropdownOpen = !m_isStageTypeDropdownOpen;
            }
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
                    if (SimpleGUI::Button(name, Vec2{this->getPanelX() + 15, y}, 260))
                    {
                        this->m_service->setCurrentStageType(type);
                        m_isStageTypeDropdownOpen = false;
                    }
                    y += 30;
                }
                y += 10;
            }
            
            return y;
        }
        
        int drawGroundSideInfo(int y) const
        {
            SimpleGUI::Headline(U"接地面設定", Vec2{this->getPanelX() + 10, y});
            y += 40;
            
            const auto currentType = this->m_service->getCurrentStageType();
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
            else
            {
                groundSideText = U"接地判定なし";
            }
            
            this->m_smallFont(groundSideText).draw(this->getPanelX() + 15, y, ColorF{0.0, 1.0, 0.0});
            y += 55;
            
            return y;
        }
        
        int drawMovementSettings(int y) const
        {
            if (this->m_service->getCurrentStageType() != Domain::Stage::StageType::MovingPlatform)
                return y;
                
            SimpleGUI::Headline(U"移動設定", Vec2{this->getPanelX() + 10, y});
            y += 40;
            
            const Array<String> movementTypes = {U"横移動", U"縦移動", U"円運動"};
            const String currentMovementTypeName = movementTypes[static_cast<size_t>(this->m_service->getMovementType())];
            if (SimpleGUI::Button(currentMovementTypeName, Vec2{this->getPanelX() + 10, y}, 270))
            {
                m_isMovementTypeDropdownOpen = !m_isMovementTypeDropdownOpen;
            }
            y += 40;
            
            if (m_isMovementTypeDropdownOpen)
            {
                for (size_t i = 0; i < movementTypes.size(); ++i)
                {
                    if (SimpleGUI::Button(movementTypes[i], Vec2{this->getPanelX() + 15, y}, 260))
                    {
                        this->m_service->setMovementType(static_cast<Domain::Stage::MovementType>(i));
                        m_isMovementTypeDropdownOpen = false;
                    }
                    y += 30;
                }
                y += 15;
            }
            
            // 選択中のオブジェクトがあれば、サイズに基づいた推奨距離を表示
            const auto selectedObjects = this->m_service->getManager().getSelectedObjects();
            if (!selectedObjects.isEmpty())
            {
                const auto& obj = selectedObjects[0]->data;
                double suggestedDistance = 200.0;
                
                if (this->m_service->getMovementType() == Domain::Stage::MovementType::Horizontal)
                {
                    suggestedDistance = obj.rect.w * 2.0;
                }
                else if (this->m_service->getMovementType() == Domain::Stage::MovementType::Vertical)
                {
                    suggestedDistance = obj.rect.h * 2.0;
                }
                else if (this->m_service->getMovementType() == Domain::Stage::MovementType::Circular)
                {
                    suggestedDistance = Min(obj.rect.w, obj.rect.h);
                }
                
                this->m_smallFont(U"推奨距離: {:.0f}"_fmt(suggestedDistance))
                    .draw(this->getPanelX() + 15, y, ColorF{0.5, 1.0, 0.5});
                y += 25;
                
                if (SimpleGUI::Button(U"サイズに合わせる", Vec2{this->getPanelX() + 10, y}, 130))
                {
                    this->m_service->setMovementDistance(suggestedDistance);
                }
                y += 40;
            }
            
            double distance = this->m_service->getMovementDistance();
            if (SimpleGUI::Slider(U"距離: {:.0f}"_fmt(distance), distance, 50.0, 1000.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
            {
                this->m_service->setMovementDistance(distance);
            }
            y += 40;
            
            double speed = this->m_service->getMovementSpeed();
            if (SimpleGUI::Slider(U"速度: {:.0f}"_fmt(speed), speed, 10.0, 500.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
            {
                this->m_service->setMovementSpeed(speed);
            }
            y += 40;
            
            bool loopMovement = this->m_service->getLoopMovement();
            if (SimpleGUI::CheckBox(loopMovement, U"ループ移動", Vec2{this->getPanelX() + 10, y}, 270))
            {
                this->m_service->setLoopMovement(loopMovement);
            }
            y += 45;
            
            return y;
        }
        
        int drawDamageSettings(int y) const
        {
            if (this->m_service->getCurrentStageType() != Domain::Stage::StageType::DamagePlatform &&
                this->m_service->getCurrentStageType() != Domain::Stage::StageType::MovingDamagePlatform)
                return y;
                
            SimpleGUI::Headline(U"ダメージ設定", Vec2{this->getPanelX() + 10, y});
            y += 40;
            
            double damageAmount = this->m_service->getDamageAmount();
            if (SimpleGUI::Slider(U"ダメージ: {:.0f}"_fmt(damageAmount), damageAmount, 1.0, 50.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
            {
                this->m_service->setDamageAmount(Math::Round(damageAmount));
            }
            y += 45;
            
            return y;
        }
        
        int drawMetadataEdit(int y) const
        {
            SimpleGUI::Headline(U"識別名", Vec2{this->getPanelX() + 10, y});
            y += 40;
            
            if (m_metadataTextEdit.text != this->m_service->getMetadata())
            {
                m_metadataTextEdit.text = this->m_service->getMetadata();
            }
            
            if (SimpleGUI::TextBox(m_metadataTextEdit, Vec2{this->getPanelX() + 10, y}, 270, 20))
            {
                this->m_service->setMetadata(m_metadataTextEdit.text);
            }
            y += 45;
            
            return y;
        }
        
        int drawSelectionInfo(int y) const
        {
            const Array<const Domain::Editor::StageEditorObjectNew*> selectedObjects = this->m_service->getManager().getSelectedObjects();
            if (selectedObjects.isEmpty())
                return y;
                
            SimpleGUI::Headline(U"選択中", Vec2{this->getPanelX() + 10, y});
            y += 25;
            
            if (selectedObjects.size() == 1)
            {
                const auto* selected = selectedObjects[0];
                this->m_smallFont(U"位置: ({:.0f}, {:.0f})"_fmt(selected->data.rect.x, selected->data.rect.y))
                    .draw(this->getPanelX() + 15, y, Palette::White);
                y += 20;
                this->m_smallFont(U"サイズ: ({:.0f}, {:.0f})"_fmt(selected->data.rect.w, selected->data.rect.h))
                    .draw(this->getPanelX() + 15, y, Palette::White);
                y += 30;
            }
            else
            {
                this->m_smallFont(U"{} 個のオブジェクトを選択中"_fmt(selectedObjects.size()))
                    .draw(this->getPanelX() + 15, y, Palette::White);
                y += 30;
            }
            
            return y;
        }
        
        void drawObjects(const Camera2D& camera, const Array<Domain::Editor::StageEditorObjectNew>& objects) const;
        void drawMovementGuide(const Domain::Stage::StageObject& obj) const;
        
        void drawGridForStage(const Camera2D& camera) const
        {
            const int gridSize = this->m_service->getSettings().getGridSize();
            const Vec2 center = camera.getCenter();
            const double scale = camera.getScale();
            const double viewWidth = Scene::Width() / scale;
            const double viewHeight = Scene::Height() / scale;
            
            const int startX = static_cast<int>((center.x - viewWidth / 2) / gridSize) - 1;
            const int endX = static_cast<int>((center.x + viewWidth / 2) / gridSize) + 1;
            const int startY = static_cast<int>((center.y - viewHeight / 2) / gridSize) - 1;
            const int endY = static_cast<int>((center.y + viewHeight / 2) / gridSize) + 1;
            
            const ColorF gridColor{0.3, 0.3, 0.3, 0.5};
            
            for (int x = startX; x <= endX; ++x) {
                double xPos = x * gridSize;
                Line{xPos, startY * gridSize, xPos, endY * gridSize}.draw(0.5, gridColor);
            }
            
            for (int y = startY; y <= endY; ++y) {
                double yPos = y * gridSize;
                Line{startX * gridSize, yPos, endX * gridSize, yPos}.draw(0.5, gridColor);
            }
            
            // 原点の軸
            Line{0, startY * gridSize, 0, endY * gridSize}.draw(2.0, Palette::Red);
            Line{startX * gridSize, 0, endX * gridSize, 0}.draw(2.0, Palette::Green);
        }
        
        ColorF getGroundSideColor(Domain::Stage::GroundSide side) const
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
        
        String getStageTypeName(Domain::Stage::StageType type) const
        {
            switch (type)
            {
            case Domain::Stage::StageType::Normal: return U"通常床";
            case Domain::Stage::StageType::MovingPlatform: return U"動く床";
            case Domain::Stage::StageType::OneWayPlatform: return U"すり抜け床";
            case Domain::Stage::StageType::DamagePlatform: return U"ダメージ床";
            case Domain::Stage::StageType::MovingDamagePlatform: return U"動くダメージ床";
            default: return U"不明";
            }
        }
    };
    
    inline void StageEditorRenderer::drawObjects(const Camera2D& camera, const Array<Domain::Editor::StageEditorObjectNew>& objects) const
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
            if (obj.data.rect.intersects(viewRect))
            {
                drawObject(obj, obj.isSelected);
                
                if (obj.data.type == Domain::Stage::StageType::MovingPlatform)
                {
                    drawMovementGuide(obj.data);
                }
            }
        }
    }
    
    inline void StageEditorRenderer::drawObject(const Domain::Editor::StageEditorObjectNew& obj, bool isSelected) const
    {
        const RectF& rect = obj.data.rect;
        
        ColorF fillColor;
        if (isSelected)
        {
            fillColor = ColorF{1.0, 1.0, 0.0, 0.3};
        }
        else
        {
            switch (obj.data.type)
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
        
        switch (obj.data.groundSide)
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
    
    inline void StageEditorRenderer::drawMovementGuide(const Domain::Stage::StageObject& obj) const
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
            
            // ループする場合は塗りつぶした円、しない場合は中空の円
            if (obj.loopMovement)
            {
                Circle{startPos, 5.0}.draw(guideColor);
                Circle{endPos, 5.0}.draw(guideColor);
            }
            else
            {
                Circle{startPos, 5.0}.drawFrame(2.0, guideColor);
                Circle{endPos, 5.0}.drawFrame(2.0, guideColor);
                // 往復を示す矢印
                Triangle{startPos.movedBy(-8, 0), 8, 0_deg}.draw(guideColor);
                Triangle{endPos.movedBy(8, 0), 8, 180_deg}.draw(guideColor);
            }
            break;
            
        case Domain::Stage::MovementType::Vertical:
            startPos = Vec2{center.x, center.y - distance / 2};
            endPos = Vec2{center.x, center.y + distance / 2};
            
            Line{startPos, endPos}.draw(3.0, guideColor);
            
            // ループする場合は塗りつぶした円、しない場合は中空の円
            if (obj.loopMovement)
            {
                Circle{startPos, 5.0}.draw(guideColor);
                Circle{endPos, 5.0}.draw(guideColor);
            }
            else
            {
                Circle{startPos, 5.0}.drawFrame(2.0, guideColor);
                Circle{endPos, 5.0}.drawFrame(2.0, guideColor);
                // 往復を示す矢印
                Triangle{startPos.movedBy(0, -8), 8, 270_deg}.draw(guideColor);
                Triangle{endPos.movedBy(0, 8), 8, 90_deg}.draw(guideColor);
            }
            break;
            
        case Domain::Stage::MovementType::Circular:
            // 円運動の場合、ループ表示は異なる方法で
            if (obj.loopMovement)
            {
                Circle{center, distance}.drawFrame(2.0, guideColor);
            }
            else
            {
                Circle{center, distance}.drawFrame(2.0, 0.0, guideColor.withAlpha(0.5));
                // 非ループを示すマーカー
                Circle{center.movedBy(distance, 0), 5.0}.drawFrame(2.0, guideColor);
            }
            break;
        }
    }
}
