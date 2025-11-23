#include "StageEditorRenderer.h"
#include "../../TextureManager.h"

namespace Jam::Presentation::Editor
{
    int StageEditorRenderer::drawCurrentMode(int y) const
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
    
    int StageEditorRenderer::drawStageTypeSelector(int y) const
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
    
    int StageEditorRenderer::drawTextureSelector(int y) const
    {
        SimpleGUI::Headline(U"テクスチャ", Vec2{this->getPanelX() + 10, y});
        y += 30;
        
        // 利用可能なテクスチャのリスト（Assets/Stageフォルダ内の画像ファイル）
        const Array<std::pair<String, String>> textures = {
            {U"なし", U""},
            {U"通常ステージ", U"Assets/Stage/normal_stage.png"},
            {U"動く床", U"Assets/Stage/moving_platform.png"},
            {U"ダメージ床", U"Assets/Stage/damage_Stage.jpg"},
            {U"ゴール", U"Assets/Stage/Goal.png"}
        };
        
        String currentTextureName = U"なし";
        const String& currentPath = this->m_service->getTexturePath();
        for (const auto& [name, path] : textures)
        {
            if (path == currentPath)
            {
                currentTextureName = name;
                break;
            }
        }
        
        if (SimpleGUI::Button(currentTextureName, Vec2{this->getPanelX() + 10, y}, 270))
        {
            m_isTextureDropdownOpen = !m_isTextureDropdownOpen;
        }
        y += 35;
        
        if (m_isTextureDropdownOpen)
        {
            for (const auto& [name, path] : textures)
            {
                if (SimpleGUI::Button(name, Vec2{this->getPanelX() + 15, y}, 260))
                {
                    this->m_service->setTexturePath(path);
                    m_isTextureDropdownOpen = false;
                }
                y += 30;
            }
            y += 10;
        }
        
        return y;
    }
    
    int StageEditorRenderer::drawGroundSideInfo(int y) const
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
    
    int StageEditorRenderer::drawMovementSettings(int y) const
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
        
        // 自動計算チェックボックス
        bool autoCalc = this->m_service->getAutoCalculateDistance();
        if (SimpleGUI::CheckBox(autoCalc, U"サイズから自動計算", Vec2{this->getPanelX() + 10, y}, 270))
        {
            this->m_service->setAutoCalculateDistance(autoCalc);
        }
        y += 35;
        
        // 選択中のオブジェクトがあれば、サイズに基づいた推奨距離を表示
        const auto selectedObjects = this->m_service->getManager().getSelectedObjects();
        if (!selectedObjects.isEmpty() && autoCalc)
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
                // 円運動の半径：床の長辺を基準とする
                suggestedDistance = Max(obj.rect.w, obj.rect.h);
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
        
        // 距離設定（スライダー + テキスト入力）
        double distance = this->m_service->getMovementDistance();
        if (SimpleGUI::Slider(U"距離: {:.0f}"_fmt(distance), distance, 50.0, 1000.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
        {
            this->m_service->setMovementDistance(distance);
            m_distanceTextEdit.text = Format(static_cast<int>(distance));
        }
        y += 35;
        
        // テキスト入力フィールド
        if (m_distanceTextEdit.text.isEmpty() || !m_distanceTextEdit.active)
        {
            m_distanceTextEdit.text = Format(static_cast<int>(this->m_service->getMovementDistance()));
        }
        
        const int distanceTextY = y;
        if (SimpleGUI::TextBox(m_distanceTextEdit, Vec2{this->getPanelX() + 10, y}, 120))
        {
            if (auto value = ParseOpt<double>(m_distanceTextEdit.text))
            {
                this->m_service->setMovementDistance(Clamp(*value, 50.0, 1000.0));
            }
        }
        
        if (m_distanceTextEdit.active)
        {
            if (KeyEnter.down() || KeyEscape.down())
            {
                m_distanceTextEdit.active = false;
            }
            else if (MouseL.down())
            {
                const RectF textBoxRect(this->getPanelX() + 10, distanceTextY, 120, 36);
                if (!textBoxRect.contains(Cursor::Pos()))
                {
                    m_distanceTextEdit.active = false;
                }
            }
        }
        y += 40;
        
        // 速度設定（スライダー + テキスト入力）
        double speed = this->m_service->getMovementSpeed();
        if (SimpleGUI::Slider(U"速度: {:.0f}"_fmt(speed), speed, 10.0, 500.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
        {
            this->m_service->setMovementSpeed(speed);
            m_speedTextEdit.text = Format(static_cast<int>(speed));
        }
        y += 35;
        
        // テキスト入力フィールド
        if (m_speedTextEdit.text.isEmpty() || !m_speedTextEdit.active)
        {
            m_speedTextEdit.text = Format(static_cast<int>(this->m_service->getMovementSpeed()));
        }
        
        const int speedTextY = y;
        if (SimpleGUI::TextBox(m_speedTextEdit, Vec2{this->getPanelX() + 10, y}, 120))
        {
            if (auto value = ParseOpt<double>(m_speedTextEdit.text))
            {
                this->m_service->setMovementSpeed(Clamp(*value, 10.0, 500.0));
            }
        }
        
        if (m_speedTextEdit.active)
        {
            if (KeyEnter.down() || KeyEscape.down())
            {
                m_speedTextEdit.active = false;
            }
            else if (MouseL.down())
            {
                const RectF textBoxRect(this->getPanelX() + 10, speedTextY, 120, 36);
                if (!textBoxRect.contains(Cursor::Pos()))
                {
                    m_speedTextEdit.active = false;
                }
            }
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
    
    int StageEditorRenderer::drawDamageSettings(int y) const
    {
        if (this->m_service->getCurrentStageType() != Domain::Stage::StageType::DamagePlatform &&
            this->m_service->getCurrentStageType() != Domain::Stage::StageType::MovingDamagePlatform)
            return y;
            
        SimpleGUI::Headline(U"ダメージ設定", Vec2{this->getPanelX() + 10, y});
        y += 40;
        
        // ダメージ設定（スライダー + テキスト入力）
        double damageAmount = this->m_service->getDamageAmount();
        if (SimpleGUI::Slider(U"ダメージ: {:.0f}"_fmt(damageAmount), damageAmount, 1.0, 50.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
        {
            this->m_service->setDamageAmount(Math::Round(damageAmount));
            m_damageTextEdit.text = Format(static_cast<int>(damageAmount));
        }
        y += 35;
        
        // テキスト入力フィールド
        if (m_damageTextEdit.text.isEmpty() || !m_damageTextEdit.active)
        {
            m_damageTextEdit.text = Format(static_cast<int>(this->m_service->getDamageAmount()));
        }
        
        const int damageTextY = y;
        if (SimpleGUI::TextBox(m_damageTextEdit, Vec2{this->getPanelX() + 10, y}, 120))
        {
            if (auto value = ParseOpt<double>(m_damageTextEdit.text))
            {
                this->m_service->setDamageAmount(Math::Round(Clamp(*value, 1.0, 50.0)));
            }
        }
        
        if (m_damageTextEdit.active)
        {
            if (KeyEnter.down() || KeyEscape.down())
            {
                m_damageTextEdit.active = false;
            }
            else if (MouseL.down())
            {
                const RectF textBoxRect(this->getPanelX() + 10, damageTextY, 120, 36);
                if (!textBoxRect.contains(Cursor::Pos()))
                {
                    m_damageTextEdit.active = false;
                }
            }
        }
        y += 45;
        
        return y;
    }
    
    int StageEditorRenderer::drawMetadataEdit(int y) const
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
        
        // Enterキー、Escキー、またはテキストボックス外をクリックでフォーカスを外す
        if (m_metadataTextEdit.active)
        {
            if (KeyEnter.down() || KeyEscape.down())
            {
                m_metadataTextEdit.active = false;
            }
            // テキストボックス外をクリックした場合もフォーカスを外す
            else if (MouseL.down())
            {
                const RectF textBoxRect(this->getPanelX() + 10, y, 270, 36);
                if (!textBoxRect.contains(Cursor::Pos()))
                {
                    m_metadataTextEdit.active = false;
                }
            }
        }
        
        y += 45;
        
        return y;
    }
    
    int StageEditorRenderer::drawSelectionInfo(int y) const
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
    
    void StageEditorRenderer::drawObjects(const Camera2D& camera, const Array<Domain::Editor::StageEditorObjectNew>& objects) const
    {
        const Vec2 center = camera.getCenter();
        const double scale = camera.getScale();
        const double viewWidth = Scene::Width() / scale;
        const double viewHeight = Scene::Height() / scale;
        const RectF viewRect{center.x - viewWidth / 2, center.y - viewHeight / 2, viewWidth, viewHeight};
        
        // プレイヤースポーン位置を描画
        const Vec2 playerSpawnPos = this->m_service->getPlayerSpawnPosition();
        const double playerRadius = 20.0;
        
        // 選択状態やドラッグ中で色を変える
        ColorF spawnColor = ColorF{0.0, 1.0, 1.0, 0.5};  // デフォルト: シアン
        ColorF borderColor = ColorF{0.0, 1.0, 1.0};
        
        if (this->m_service->isSpawnSelected())
        {
            spawnColor = ColorF{1.0, 1.0, 0.0, 0.6};  // 選択中: 黄色
            borderColor = ColorF{1.0, 1.0, 0.0};
        }
        
        Circle{playerSpawnPos, playerRadius}.draw(spawnColor);
        Circle{playerSpawnPos, playerRadius}.drawFrame(this->m_service->isSpawnSelected() ? 3.0 : 2.0, borderColor);
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
    
    void StageEditorRenderer::drawObject(const Domain::Editor::StageEditorObjectNew& obj, bool isSelected) const
    {
        const RectF& rect = obj.data.rect;
        
        // テクスチャがある場合は描画
        bool hasTexture = false;
        if (!obj.data.texturePath.isEmpty())
        {
            try
            {
                const auto& texture = TextureManager::Load(obj.data.texturePath);
                if (texture)
                {
                    // ゴールテクスチャの場合は矩形サイズに合わせて拡大縮小
                    if (obj.data.texturePath.includes(U"Goal.png") || obj.data.texturePath.includes(U"goal.png"))
                    {
                        texture.resized(rect.size).draw(rect.pos);
                    }
                    else
                    {
                        // その他のテクスチャはタイル表示
                        const ScopedRenderStates2D sampler{ SamplerState::RepeatLinear };
                        texture.mapped(rect.size).draw(rect.pos);
                    }
                    hasTexture = true;
                }
            }
            catch (...)
            {
                // テクスチャロードに失敗した場合は通常描画にフォールバック
            }
        }
        
        // テクスチャがない場合は通常の色で描画
        if (!hasTexture)
        {
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
        }
        
        // 接地面の表示
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
        
        // 選択時のフレーム
        if (isSelected)
        {
            rect.drawFrame(3.0, 0.0, Palette::Yellow);
        }
    }
    
    void StageEditorRenderer::drawMovementGuide(const Domain::Stage::StageObject& obj) const
    {
        const Vec2 center = obj.rect.center();
        const double distance = obj.movementDistance;
        // ループのオン/オフで色を変える（オレンジ：ループ、シアン：往復）
        const ColorF guideColor = obj.loopMovement ? ColorF{1.0, 0.5, 0.0, 0.8} : ColorF{0.0, 1.0, 1.0, 0.8};
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
        case Domain::Stage::StageType::Normal: return U"通常床";
        case Domain::Stage::StageType::MovingPlatform: return U"動く床";
        case Domain::Stage::StageType::OneWayPlatform: return U"すり抜け床";
        case Domain::Stage::StageType::DamagePlatform: return U"ダメージ床";
        case Domain::Stage::StageType::MovingDamagePlatform: return U"動くダメージ床";
        default: return U"不明";
        }
    }
}
