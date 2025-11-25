#include "StageEditorRenderer.h"
#include "../../TextureManager.h"
#include "../Utilities/EditorTextInputUtil.h"

namespace Jam::Presentation::Editor
{
    int StageEditorRenderer::drawCurrentMode(int y) const
    {
        y = this->drawSectionHeader(U"現在のモード", y);
        
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
        
        this->m_font(modeName).draw(this->getPanelX() + GUI_PADDING_15, y, modeColor);
        y += GUI_SPACING_45;
        
        return y;
    }
    
    int StageEditorRenderer::drawStageTypeSelector(int y) const
    {
        y = this->drawSectionHeader(U"ステージタイプ", y);
        
        const auto currentStageType = this->m_service->getCurrentStageType();
        
        const String currentTypeName = getStageTypeName(currentStageType);
        if (SimpleGUI::Button(currentTypeName, Vec2{this->getPanelX() + GUI_PADDING, y}, GUI_BUTTON_WIDTH))
        {
            m_isStageTypeDropdownOpen = !m_isStageTypeDropdownOpen;
        }
        y += GUI_SPACING_35;
        
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
                if (SimpleGUI::Button(name, Vec2{this->getPanelX() + GUI_PADDING_15, y}, GUI_DROPDOWN_BUTTON_WIDTH))
                {
                    this->m_service->setCurrentStageType(type);
                    m_isStageTypeDropdownOpen = false;
                }
                y += GUI_SPACING_30;
            }
            y += GUI_PADDING;
        }
        
        y += this->getSmallSpacing();
        return y;
    }
    
    int StageEditorRenderer::drawTextureSelector(int y) const
    {
        y = this->drawSectionHeader(U"テクスチャ", y);
        
        // 現在のStageTypeを取得
        const auto currentStageType = this->m_service->getCurrentStageType();
        
        // StageTypeに応じて利用可能なテクスチャをフィルタリング
        Array<std::pair<String, String>> textures;
        
        switch (currentStageType) {
            case Domain::Stage::StageType::Normal:
            case Domain::Stage::StageType::OneWayPlatform:
                textures.push_back({U"通常ステージ", U"Assets/Stage/normal_stage.png"});
                textures.push_back({U"通常ステージ2", U"Assets/Stage/normal_stage2.png"});
                break;
                
            case Domain::Stage::StageType::MovingPlatform:
                textures.push_back({U"動く床", U"Assets/Stage/moving_platform.png"});
                break;
                
            case Domain::Stage::StageType::DamagePlatform:
            case Domain::Stage::StageType::MovingDamagePlatform:
                textures.push_back({U"ダメージ床", U"Assets/Stage/damage_Stage.jpg"});
                break;
                
            default:
                break;
        }
        
        // デフォルトテクスチャ名を設定（最初のテクスチャまたは空）
        String currentTextureName = textures.empty() ? U"テクスチャなし" : textures[0].first;
        const String& currentPath = this->m_service->getTexturePath();
        for (const auto& [name, path] : textures)
        {
            if (path == currentPath)
            {
                currentTextureName = name;
                break;
            }
        }
        
        if (SimpleGUI::Button(currentTextureName, Vec2{this->getPanelX() + GUI_PADDING, y}, GUI_BUTTON_WIDTH))
        {
            m_isTextureDropdownOpen = !m_isTextureDropdownOpen;
        }
        y += GUI_SPACING_35;
        
        if (m_isTextureDropdownOpen)
        {
            for (const auto& [name, path] : textures)
            {
                if (SimpleGUI::Button(name, Vec2{this->getPanelX() + GUI_PADDING_15, y}, GUI_DROPDOWN_BUTTON_WIDTH))
                {
                    this->m_service->setTexturePath(path);
                    m_isTextureDropdownOpen = false;
                }
                y += GUI_SPACING_30;
            }
            y += GUI_PADDING;
        }
        
        y += this->getSmallSpacing();
        return y;
    }
    
    int StageEditorRenderer::drawOtherObjectsSelector(int y) const
    {
        y = this->drawSectionHeader(U"その他のオブジェクト", y);
        
        const auto currentOtherType = this->m_service->getOtherObjectType();
        
        // ゴールボタン（実装中のためコメントアウト）
        /*
        const bool isGoalSelected = (currentOtherType == UseCase::Editor::OtherObjectType::Goal);
        
        if (SimpleGUI::Button(U"ゴール", Vec2{this->getPanelX() + 10, y}, 270))
        {
            if (isGoalSelected)
            {
                // 既に選択されている場合は解除
                this->m_service->setOtherObjectType(UseCase::Editor::OtherObjectType::None);
            }
            else
            {
                // 選択状態にする
                this->m_service->setOtherObjectType(UseCase::Editor::OtherObjectType::Goal);
            }
        }
        y += 40;
        */
        
        // 記憶のかけらボタン（実装中のためコメントアウト）
        /*
        const bool isFlagmentSelected = (currentOtherType == UseCase::Editor::OtherObjectType::FlagmentMemory);
        
        if (SimpleGUI::Button(U"記憶のかけら", Vec2{this->getPanelX() + 10, y}, 270))
        {
            if (isFlagmentSelected)
            {
                // 既に選択されている場合は解除
                this->m_service->setOtherObjectType(UseCase::Editor::OtherObjectType::None);
            }
            else
            {
                // 選択状態にする
                this->m_service->setOtherObjectType(UseCase::Editor::OtherObjectType::FlagmentMemory);
            }
        }
        y += 40;
        */
        
        y += this->getSmallSpacing();
        return y;
    }
    
    int StageEditorRenderer::drawMovementSettings(int y) const
    {
        if (this->m_service->getCurrentStageType() != Domain::Stage::StageType::MovingPlatform)
            return y;
            
        y = this->drawSectionHeader(U"移動設定", y);
        
        const Array<String> movementTypes = {U"横移動", U"縦移動", U"円運動"};
        const String currentMovementTypeName = movementTypes[static_cast<size_t>(this->m_service->getMovementType())];
        if (SimpleGUI::Button(currentMovementTypeName, Vec2{this->getPanelX() + GUI_PADDING, y}, GUI_BUTTON_WIDTH))
        {
            m_isMovementTypeDropdownOpen = !m_isMovementTypeDropdownOpen;
        }
        y += GUI_SPACING_40;
        
        if (m_isMovementTypeDropdownOpen)
        {
            for (size_t i = 0; i < movementTypes.size(); ++i)
            {
                if (SimpleGUI::Button(movementTypes[i], Vec2{this->getPanelX() + GUI_PADDING_15, y}, GUI_DROPDOWN_BUTTON_WIDTH))
                {
                    this->m_service->setMovementType(static_cast<Domain::Stage::MovementType>(i));
                    m_isMovementTypeDropdownOpen = false;
                }
                y += GUI_SPACING_30;
            }
            y += GUI_PADDING_15;
        }
        
        // 自動計算チェックボックス
        bool autoCalc = this->m_service->getAutoCalculateDistance();
        if (SimpleGUI::CheckBox(autoCalc, U"サイズから自動計算", Vec2{this->getPanelX() + GUI_PADDING, y}, GUI_BUTTON_WIDTH))
        {
            this->m_service->setAutoCalculateDistance(autoCalc);
        }
        y += GUI_SPACING_35;
        
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
                .draw(this->getPanelX() + GUI_PADDING_15, y, Palette::Lightgreen);
            y += GUI_SPACING_25;
            
            if (SimpleGUI::Button(U"サイズに合わせる", Vec2{this->getPanelX() + GUI_PADDING, y}, GUI_SMALL_BUTTON_WIDTH))
            {
                this->m_service->setMovementDistance(suggestedDistance);
            }
            y += GUI_SPACING_40;
        }
        
        // 距離設定 - EditorTextInputUtilを使用
        double distance = this->m_service->getMovementDistance();
        int distanceInt = static_cast<int>(distance);
        y = EditorTextInputUtil::drawIntSlider(
            U"距離",
            distanceInt,
            50, 1000,
            Vec2{this->getPanelX() + GUI_PADDING, static_cast<double>(y)},
            m_distanceTextEdit
        );
        this->m_service->setMovementDistance(static_cast<double>(distanceInt));
        
        
        // 速度設定 - EditorTextInputUtilを使用
        double speed = this->m_service->getMovementSpeed();
        int speedInt = static_cast<int>(speed);
        y = EditorTextInputUtil::drawIntSlider(
            U"速度",
            speedInt,
            10, 500,
            Vec2{this->getPanelX() + GUI_PADDING, static_cast<double>(y)},
            m_speedTextEdit
        );
        this->m_service->setMovementSpeed(static_cast<double>(speedInt));
        
        
        bool loopMovement = this->m_service->getLoopMovement();
        if (SimpleGUI::CheckBox(loopMovement, U"ループ移動", Vec2{this->getPanelX() + GUI_PADDING, y}, GUI_BUTTON_WIDTH))
        {
            this->m_service->setLoopMovement(loopMovement);
        }
        y += GUI_SPACING_40;
        
        y += this->getSmallSpacing();
        return y;
    }
    
    int StageEditorRenderer::drawDamageSettings(int y) const
    {
        if (this->m_service->getCurrentStageType() != Domain::Stage::StageType::DamagePlatform &&
            this->m_service->getCurrentStageType() != Domain::Stage::StageType::MovingDamagePlatform)
            return y;
            
        y = this->drawSectionHeader(U"ダメージ設定", y);
        
        // ダメージ設定 - EditorTextInputUtilを使用
        double damageAmount = this->m_service->getDamageAmount();
        int damageInt = static_cast<int>(damageAmount);
        y = EditorTextInputUtil::drawIntSlider(
            U"ダメージ",
            damageInt,
            1, 50,
            Vec2{this->getPanelX() + GUI_PADDING, static_cast<double>(y)},
            m_damageTextEdit
        );
        this->m_service->setDamageAmount(Math::Round(static_cast<double>(damageInt)));
        
        
        y += this->getSmallSpacing();
        return y;
    }
    
    int StageEditorRenderer::drawMetadataEdit(int y) const
    {
        return y;
    }
    
    int StageEditorRenderer::drawSelectionInfo(int y) const
    {
        const Array<const Domain::Editor::StageEditorObject*> selectedObjects = this->m_service->getManager().getSelectedObjects();
        if (selectedObjects.isEmpty())
            return y;
            
        y = this->drawSectionHeader(U"選択中", y);
        
        if (selectedObjects.size() == 1)
        {
            const auto* selected = selectedObjects[0];
            this->m_smallFont(U"位置: ({:.0f}, {:.0f})"_fmt(selected->data.rect.x, selected->data.rect.y))
                .draw(this->getPanelX() + GUI_PADDING_15, y, Palette::White);
            y += GUI_SPACING_20;
            this->m_smallFont(U"サイズ: ({:.0f}, {:.0f})"_fmt(selected->data.rect.w, selected->data.rect.h))
                .draw(this->getPanelX() + GUI_PADDING_15, y, Palette::White);
            y += GUI_SPACING_30;
        }
        else
        {
            this->m_smallFont(U"{} 個のオブジェクトを選択中"_fmt(selectedObjects.size()))
                .draw(this->getPanelX() + GUI_PADDING_15, y, Palette::White);
            y += GUI_SPACING_30;
        }
        
        return y;
    }
    
    void StageEditorRenderer::drawObjects(const Camera2D& camera, const Array<Domain::Editor::StageEditorObject>& objects) const
    {
        const Vec2 center = camera.getCenter();
        const double scale = camera.getScale();
        const double viewWidth = Scene::Width() / scale;
        const double viewHeight = Scene::Height() / scale;
        const RectF viewRect{center.x - viewWidth / 2, center.y - viewHeight / 2, viewWidth, viewHeight};
        
        // プレイヤースポーン位置を描画
        const Vec2 playerSpawnPos = this->m_service->getPlayerSpawnPosition();
        const double playerRadius = PLAYER_SPAWN_RADIUS;
        
        // 選択状態やドラッグ中で色を変える
        ColorF spawnColor = Palette::Cyan.withAlpha(0.5);  // デフォルト: シアン
        ColorF borderColor = Palette::Cyan;
        
        if (this->m_service->isSpawnSelected())
        {
            spawnColor = Palette::Yellow.withAlpha(0.6);  // 選択中: 黄色
            borderColor = Palette::Yellow;
        }
        
        Circle{playerSpawnPos, playerRadius}.draw(spawnColor);
        Circle{playerSpawnPos, playerRadius}.drawFrame(this->m_service->isSpawnSelected() ? 3.0 : FRAME_THICKNESS, borderColor);
        Circle{playerSpawnPos, 5.0}.draw(Palette::White);
        
        for (const auto& obj : objects)
        {
            if (obj.data.rect.intersects(viewRect))
            {
                drawObject(obj, obj.isSelected);
                
                if (obj.data.type == Domain::Stage::StageType::MovingPlatform)
                    drawMovementGuide(obj.data);
            }
        }
    }
    
    void StageEditorRenderer::drawObject(const Domain::Editor::StageEditorObject& obj, bool isSelected) const
    {
        const RectF& rect = obj.data.rect;
        
        // テクスチャがある場合は描画
        bool hasTexture = false;
        if (!obj.data.texturePath.isEmpty())
        {
            const auto& texture = TextureManager::Load(obj.data.texturePath);
            
            // テクスチャが読み込めたか明示的にチェック
            if (texture)
            {
                // ゴールテクスチャとフラグメントテクスチャの場合は矩形サイズに合わせて拡大縮小
                if (obj.data.texturePath.includes(U"Goal.png") || obj.data.texturePath.includes(U"goal.png") ||
                    obj.data.texturePath.includes(U"FlagmentMemory.png"))
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
            else
                assert(false && "Failed to load texture. Check if the texture file exists.");
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
        
        // ゲーム側の実装に合わせて、床の中心の移動範囲を表示
        // MovingPlatformStageでは m_currentOffset = progress * m_movementDistance (progress: 0.0~1.0)
        // 実際の床の中心位置 = baseRect.center() + offset
        // つまり、床の中心が center から center + (distance, 0) または (0, distance) まで移動
        Vec2 startPos, endPos;
        
        switch (obj.movementType)
        {
        case Domain::Stage::MovementType::Horizontal:
            // 横移動: 床の中心X座標が center.x から center.x + distance まで
            startPos = Vec2{center.x, center.y};
            endPos = Vec2{center.x + distance, center.y};
            
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
            // 縦移動: 床の中心Y座標が center.y から center.y + distance まで
            startPos = Vec2{center.x, center.y};
            endPos = Vec2{center.x, center.y + distance};
            
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
            // 円運動: 床の中心が center を中心に distance の半径で回転
            // offset = (Cos(angle) * distance, Sin(angle) * distance)
            // 床の中心位置 = center + offset
            if (obj.loopMovement)
            {
                Circle{center, distance}.drawFrame(2.0, guideColor);
            }
            else
            {
                Circle{center, distance}.drawFrame(2.0, 0.0, guideColor.withAlpha(0.5));
                // 非ループを示すマーカー(初期位置: 右方向)
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