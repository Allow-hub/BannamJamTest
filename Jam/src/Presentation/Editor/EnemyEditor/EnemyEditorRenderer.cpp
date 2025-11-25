#include "EnemyEditorRenderer.h"
#include "../Utilities/EditorTextInputUtil.h"

namespace Jam::Presentation::Editor
{
    // StageEditorManagerへの参照を設定（プレイヤースポーン位置表示用）
    void EnemyEditorRenderer::setStageManager(const Domain::Editor::StageEditorManager* manager)
    {
        m_stageManager = manager;
    }

    // 敵エディタのビューを描画（グリッド、プレイヤースポーン、敵）
    void EnemyEditorRenderer::drawView() const
    {
        const auto& camera = this->m_service->getCamera();
        auto transformer = camera.createTransformer();
        
        // グリッド描画
        EditorGridUtil::drawGridWithAxes(camera, this->m_service->getSettings().getGridSize());
        
        // StageEditorManagerからスポーン位置を取得
        Vec2 playerSpawnPos{0, 0};
        if (m_stageManager)
            playerSpawnPos = m_stageManager->getPlayerSpawnPosition();
        
        const double playerRadius = PLAYER_SPAWN_RADIUS;
        Circle{playerSpawnPos, playerRadius}.draw(Palette::Cyan.withAlpha(0.5));
        Circle{playerSpawnPos, playerRadius}.drawFrame(FRAME_THICKNESS, Palette::Cyan);
        Circle{playerSpawnPos, PLAYER_SPAWN_CENTER_RADIUS}.draw(Palette::White);
        
        for (const auto& enemy : this->m_service->getManager().getAllObjects())
        {
            drawEnemy(enemy, enemy.isSelected);
            if (!enemy.data.isBoss())
                drawPatrolRange(enemy.data);
        }
    }

    // 敵エディタのGUIパネルを描画
    void EnemyEditorRenderer::drawGUIPanel() const
    {
        this->handlePanelScroll();
        this->drawBasicPanel();
        
        // 折りたたみボタンとタイトル（スクロール対象外）
        this->drawTitleAndToggle(U"敵エディタ");
        
        if (this->m_isPanelCollapsed) return;
        
        {
            auto transformer = this->createPanelTransformer();
            
            int y = this->getContentStartY();
            
            if (SimpleGUI::Button(U"ステージエディタに切り替え", Vec2{this->getPanelX() + GUI_PADDING, y}, GUI_BUTTON_WIDTH_WIDE))
                m_switchToStageEditor = true;

            y += GUI_SPACING_45;
            
            y = this->drawCameraSettings(y);
            y = drawCurrentMode(y);
            y = drawEnemyTypeSelector(y);
            y = drawPatrolSettings(y);
            y = drawChaseSettings(y);
        }
    }

    // テキスト入力がアクティブかチェック（カメラ操作の無効化用）
    bool EnemyEditorRenderer::isTextInputActive() const
    {
        return m_patrolDistanceTextEdit.active || m_waitTimeTextEdit.active ||
               m_foundDistanceTextEdit.active || m_attackRangeTextEdit.active ||
               m_loseRangeTextEdit.active || m_speedFactorTextEdit.active;
    }

    // 現在のエディタモードを描画
    int EnemyEditorRenderer::drawCurrentMode(int y) const
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
    
    // 敵タイプ選択UIを描画
    int EnemyEditorRenderer::drawEnemyTypeSelector(int y) const
    {
        y = this->drawSectionHeader(U"敵タイプ", y);
        
        String currentTypeName = getEnemyTypeName(this->m_service->getEnemyType());
        if (SimpleGUI::Button(currentTypeName, Vec2{this->getPanelX() + GUI_PADDING, y}, GUI_BUTTON_WIDTH_WIDE))
        {
            m_isEnemyTypeDropdownOpen = !m_isEnemyTypeDropdownOpen;
        }
        y += GUI_SPACING_40;
        
        if (m_isEnemyTypeDropdownOpen)
        {
            Array<Domain::EnemyType> types = {
                Domain::EnemyType::LittleDevil,
                Domain::EnemyType::Spider,
                Domain::EnemyType::Ribbon,
                Domain::EnemyType::Eye,
                Domain::EnemyType::Clown,
                Domain::EnemyType::GothicLolitaDoll,
                Domain::EnemyType::Boss1_3
            };
            
            for (const auto& type : types)
            {
                if (SimpleGUI::Button(getEnemyTypeName(type), Vec2{this->getPanelX() + GUI_PADDING_15, y}, GUI_DROPDOWN_BUTTON_WIDTH + 5))
                {
                    this->m_service->setEnemyType(type);
                    m_isEnemyTypeDropdownOpen = false;
                }
                y += GUI_SPACING_35;
            }
            y += GUI_PADDING;
        }
        
        y += this->getSmallSpacing();
        return y;
    }
    
    // 巡回設定UIを描画（距離、待機時間、発見距離）
    int EnemyEditorRenderer::drawPatrolSettings(int y) const
    {
        if (this->m_service->getEnemyType() == Domain::EnemyType::Boss1_3)
            return y;
            
        y = this->drawSectionHeader(U"巡回設定", y);
        
        // 巡回距離 - EditorTextInputUtilを使用
        double patrolDistance = this->m_service->getPatrolDistance();
        int patrolDistanceInt = static_cast<int>(patrolDistance);
        y = EditorTextInputUtil::drawIntSlider(
            U"距離",
            patrolDistanceInt,
            50, 500,
            Vec2{this->getPanelX() + GUI_PADDING, static_cast<double>(y)},
            m_patrolDistanceTextEdit
        );
        this->m_service->setPatrolDistance(static_cast<double>(patrolDistanceInt));
        
        // 待機時間 - EditorTextInputUtilを使用
        double waitTime = this->m_service->getPatrolWaitTime();
        y = EditorTextInputUtil::drawDoubleSlider(
            U"待機",
            waitTime,
            0.5, 5.0,
            Vec2{this->getPanelX() + GUI_PADDING, static_cast<double>(y)},
            m_waitTimeTextEdit,
            1  // 小数点1桁
        );
        this->m_service->setPatrolWaitTime(waitTime);
        
        // 発見距離 - EditorTextInputUtilを使用
        double foundDistance = this->m_service->getFoundDistance();
        int foundDistanceInt = static_cast<int>(foundDistance);
        y = EditorTextInputUtil::drawIntSlider(
            U"発見",
            foundDistanceInt,
            300, 1000,
            Vec2{this->getPanelX() + GUI_PADDING, static_cast<double>(y)},
            m_foundDistanceTextEdit
        );
        this->m_service->setFoundDistance(static_cast<double>(foundDistanceInt));
        
        y += this->getSmallSpacing();
        return y;
    }
    
    // 追跡設定UIを描画（攻撃範囲、見失い範囲、速度倍率）
    int EnemyEditorRenderer::drawChaseSettings(int y) const
    {
        if (this->m_service->getEnemyType() == Domain::EnemyType::Boss1_3 ||
            this->m_service->getEnemyType() == Domain::EnemyType::LittleDevil)
            return y;
            
        y = this->drawSectionHeader(U"追跡設定", y);
        
        // 攻撃範囲 - EditorTextInputUtilを使用
        double attackRange = this->m_service->getAttackRange();
        int attackRangeInt = static_cast<int>(attackRange);
        y = EditorTextInputUtil::drawIntSlider(
            U"攻撃範囲",
            attackRangeInt,
            300, 1000,
            Vec2{this->getPanelX() + GUI_PADDING, static_cast<double>(y)},
            m_attackRangeTextEdit
        );
        this->m_service->setAttackRange(static_cast<double>(attackRangeInt));
        
        // 見失い範囲 - EditorTextInputUtilを使用
        double loseRange = this->m_service->getLoseRange();
        int loseRangeInt = static_cast<int>(loseRange);
        y = EditorTextInputUtil::drawIntSlider(
            U"見失い範囲",
            loseRangeInt,
            500, 1500,
            Vec2{this->getPanelX() + GUI_PADDING, static_cast<double>(y)},
            m_loseRangeTextEdit
        );
        this->m_service->setLoseRange(static_cast<double>(loseRangeInt));
        
        // 速度倍率 - EditorTextInputUtilを使用
        double speedFactor = this->m_service->getMoveSpeedFactor();
        y = EditorTextInputUtil::drawDoubleSlider(
            U"速度倍率",
            speedFactor,
            1.0, 2.0,
            Vec2{this->getPanelX() + GUI_PADDING, static_cast<double>(y)},
            m_speedFactorTextEdit,
            1  // 小数点1桁
        );
        this->m_service->setMoveSpeedFactor(speedFactor);
        
        y += this->getSmallSpacing();
        return y;
    }
    
    // 敵タイプの表示名を取得
    String EnemyEditorRenderer::getEnemyTypeName(Domain::EnemyType type) const
    {
        switch (type)
        {
        case Domain::EnemyType::LittleDevil: return U"リトルデビル";
        case Domain::EnemyType::Spider: return U"スパイダー";
        case Domain::EnemyType::Ribbon: return U"リボン";
        case Domain::EnemyType::Eye: return U"アイ";
        case Domain::EnemyType::Clown: return U"クラウン";
        case Domain::EnemyType::GothicLolitaDoll: return U"ゴシックロリータ人形";
        case Domain::EnemyType::Boss1_3: return U"ボス1-3";
        default: return U"不明";
        }
    }
    
    // 敵タイプに応じた色を取得
    ColorF EnemyEditorRenderer::getEnemyColor(Domain::EnemyType type) const
    {
        switch (type)
        {
        case Domain::EnemyType::LittleDevil: return ColorF{1.0, 0.5, 0.5};
        case Domain::EnemyType::Spider: return ColorF{0.5, 0.3, 0.8};
        case Domain::EnemyType::Ribbon: return ColorF{1.0, 0.7, 0.8};
        case Domain::EnemyType::Eye: return ColorF{0.3, 0.8, 1.0};
        case Domain::EnemyType::Clown: return ColorF{0.9, 0.9, 0.3};
        case Domain::EnemyType::GothicLolitaDoll: return ColorF{0.8, 0.5, 0.8};
        case Domain::EnemyType::Boss1_3: return ColorF{1.0, 0.0, 0.0};
        default: return Palette::Gray;
        }
    }
    
    // 敵オブジェクトを描画（円と名前）
    void EnemyEditorRenderer::drawEnemy(const Domain::Editor::EnemyEditorObject& enemy, bool isSelected) const
    {
        const ColorF color = getEnemyColor(enemy.data.type);
        const double radius = enemy.data.isBoss() ? 50.0 : 30.0;
        
        Circle{enemy.data.position, radius}.draw(color.withAlpha(0.6));
        Circle{enemy.data.position, radius}.drawFrame(3.0, isSelected ? ColorF(Palette::Yellow) : color);
        
        this->m_smallFont(getEnemyTypeName(enemy.data.type))
            .drawAt(enemy.data.position.movedBy(0, radius + 15), Palette::White);
    }
    
    // 巡回範囲と追跡範囲を描画
    void EnemyEditorRenderer::drawPatrolRange(const Domain::Editor::EnemyObject& enemy) const
    {
        const double patrolDistance = enemy.patrol.patrolPoints.size() >= 2 ? 
            enemy.patrol.patrolPoints[0].position.distanceFrom(enemy.patrol.patrolPoints[1].position) : 100.0;
        
        if (enemy.patrol.patrolPoints.size() >= 2)
        {
            for (size_t i = 0; i < enemy.patrol.patrolPoints.size(); ++i)
            {
                const auto& point = enemy.patrol.patrolPoints[i];
                Circle{point.position, 8.0}.draw(ColorF{0.0, 1.0, 0.0, 0.5});
                
                if (i + 1 < enemy.patrol.patrolPoints.size())
                {
                    Line{point.position, enemy.patrol.patrolPoints[i + 1].position}
                        .draw(2.0, ColorF{0.0, 1.0, 0.0, 0.7});
                }
            }
            
            if (enemy.patrol.loop && enemy.patrol.patrolPoints.size() > 0)
            {
                Line{enemy.patrol.patrolPoints.back().position, enemy.patrol.patrolPoints.front().position}
                    .draw(2.0, ColorF{0.0, 1.0, 0.0, 0.7});
            }
        }
        
        if (enemy.chase)
        {
            Circle{enemy.position, enemy.chase->attackRange}.drawFrame(2.0, ColorF{1.0, 0.0, 0.0, 0.5});
            Circle{enemy.position, enemy.chase->loseRange}.drawFrame(2.0, ColorF{1.0, 1.0, 0.0, 0.3});
        }
    }
}