// ========================================
// EnemyEditorRenderer.h（リファクタリング版）
// ========================================
#pragma once
#include "../Base/EditorRendererBase.h"
#include "../../../UseCase/Editor/EnemyEditor/EnemyEditorService.h"

namespace Jam::Presentation::Editor
{
    class EnemyEditorRenderer : public EditorRendererBase<UseCase::Editor::EnemyEditorService>
    {
    private:
        mutable bool m_isEnemyTypeDropdownOpen = false;
        mutable bool m_switchToStageEditor = false;
        
        TextEditState m_patrolDistanceTextEdit;
        TextEditState m_patrolWaitTimeTextEdit;
        TextEditState m_foundDistanceTextEdit;
        TextEditState m_attackRangeTextEdit;
        TextEditState m_loseRangeTextEdit;
        TextEditState m_moveSpeedFactorTextEdit;
        
    public:
        void drawView() const override
        {
            const auto& camera = this->m_service->getCamera();
            auto transformer = camera.createTransformer();
            
            // グリッド描画
            drawGridForEditor(camera);
            
            const Vec2 playerSpawnPos{0, 0};
            const double playerRadius = 20.0;
            Circle{playerSpawnPos, playerRadius}.draw(ColorF{0.0, 1.0, 1.0, 0.5});
            Circle{playerSpawnPos, playerRadius}.drawFrame(2.0, ColorF{0.0, 1.0, 1.0});
            
            for (const auto& enemy : this->m_service->getManager().getAllObjects())
            {
                drawEnemy(enemy, enemy.isSelected);
                if (!enemy.data.isBoss())
                {
                    drawPatrolRange(enemy.data);
                }
            }
        }
        
        void drawGUIPanel() const override
        {
            this->handlePanelScroll();
            this->drawBasicPanel();
            
            // 折りたたみボタンとタイトル（スクロール対象外）
            this->drawTitleAndToggle(U"敵エディタ");
            
            if (this->m_isPanelCollapsed) return;
            
            {
                auto transformer = this->createPanelTransformer();
                
                int y = this->getContentStartY();
                
                if (SimpleGUI::Button(U"ステージエディタに切り替え", Vec2{this->getPanelX() + 10, y}, 280))
                {
                    m_switchToStageEditor = true;
                }
                y += 45;
                
                y = this->drawCameraSettings(y);
                y = this->drawModeDisplay(y);
                y = drawCurrentMode(y);
                y = drawEnemyTypeSelector(y);
                y = drawPatrolSettings(y);
                y = drawChaseSettings(y);
            }
        }
        
        bool isEditorSwitchRequested() const { return m_switchToStageEditor; }
        void resetEditorSwitchRequest() const { m_switchToStageEditor = false; }
        
        // StageEditorSceneから呼び出すためpublicに
        void drawEnemy(const Domain::Editor::EnemyEditorObjectNew& enemy, bool isSelected) const;
        void drawPatrolRange(const Domain::Editor::EnemyObject& enemy) const;
        
    private:
        int drawCurrentMode(int y) const
        {
            SimpleGUI::Headline(U"現在のモード", Vec2{this->getPanelX() + 10, y});
            y += 35;
            
            String modeName = U"配置モード";
            ColorF modeColor = Palette::Lime;
            
            this->m_font(modeName).draw(this->getPanelX() + 15, y, modeColor);
            y += 45;
            
            return y;
        }
        
        void drawGridForEditor(const Camera2D& camera) const
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
        
        int drawEnemyTypeSelector(int y) const
        {
            SimpleGUI::Headline(U"敵タイプ", Vec2{this->getPanelX() + 10, y});
            y += 40;
            
            String currentTypeName = getEnemyTypeName(this->m_service->getEnemyType());
            if (SimpleGUI::Button(currentTypeName, Vec2{this->getPanelX() + 10, y}, 280))
            {
                m_isEnemyTypeDropdownOpen = !m_isEnemyTypeDropdownOpen;
            }
            y += 40;
            
            if (m_isEnemyTypeDropdownOpen)
            {
                Array<Domain::Editor::EnemyType> types = {
                    Domain::Editor::EnemyType::LittleDevil,
                    Domain::Editor::EnemyType::Spider,
                    Domain::Editor::EnemyType::Ribbon,
                    Domain::Editor::EnemyType::Eye,
                    Domain::Editor::EnemyType::Clown,
                    Domain::Editor::EnemyType::Boss1_3
                };
                
                for (const auto& type : types)
                {
                    if (SimpleGUI::Button(getEnemyTypeName(type), Vec2{this->getPanelX() + 15, y}, 265))
                    {
                        this->m_service->setEnemyType(type);
                        m_isEnemyTypeDropdownOpen = false;
                    }
                    y += 35;
                }
                y += 10;
            }
            
            return y;
        }
        
        int drawPatrolSettings(int y) const
        {
            if (this->m_service->getEnemyType() == Domain::Editor::EnemyType::Boss1_3)
                return y;
                
            SimpleGUI::Headline(U"巡回設定", Vec2{this->getPanelX() + 10, y});
            y += 40;
            
            double patrolDistance = this->m_service->getPatrolDistance();
            if (SimpleGUI::Slider(U"距離: {:.0f}"_fmt(patrolDistance), patrolDistance, 50.0, 500.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
            {
                this->m_service->setPatrolDistance(patrolDistance);
            }
            y += 40;
            
            double waitTime = this->m_service->getPatrolWaitTime();
            if (SimpleGUI::Slider(U"待機: {:.1f}s"_fmt(waitTime), waitTime, 0.5, 5.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
            {
                this->m_service->setPatrolWaitTime(waitTime);
            }
            y += 40;
            
            double foundDistance = this->m_service->getFoundDistance();
            if (SimpleGUI::Slider(U"発見: {:.0f}"_fmt(foundDistance), foundDistance, 300.0, 1000.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
            {
                this->m_service->setFoundDistance(foundDistance);
            }
            y += 45;
            
            return y;
        }
        
        int drawChaseSettings(int y) const
        {
            if (this->m_service->getEnemyType() == Domain::Editor::EnemyType::Boss1_3 ||
                this->m_service->getEnemyType() == Domain::Editor::EnemyType::LittleDevil)
                return y;
                
            SimpleGUI::Headline(U"追跡設定", Vec2{this->getPanelX() + 10, y});
            y += 40;
            
            double attackRange = this->m_service->getAttackRange();
            if (SimpleGUI::Slider(U"攻撃範囲: {:.0f}"_fmt(attackRange), attackRange, 300.0, 1000.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
            {
                this->m_service->setAttackRange(attackRange);
            }
            y += 40;
            
            double loseRange = this->m_service->getLoseRange();
            if (SimpleGUI::Slider(U"見失い範囲: {:.0f}"_fmt(loseRange), loseRange, 500.0, 1500.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
            {
                this->m_service->setLoseRange(loseRange);
            }
            y += 40;
            
            double speedFactor = this->m_service->getMoveSpeedFactor();
            if (SimpleGUI::Slider(U"速度倍率: {:.1f}"_fmt(speedFactor), speedFactor, 1.0, 2.0, Vec2{this->getPanelX() + 10, y}, 100, 160))
            {
                this->m_service->setMoveSpeedFactor(speedFactor);
            }
            y += 45;
            
            return y;
        }
        
        String getEnemyTypeName(Domain::Editor::EnemyType type) const
        {
            switch (type)
            {
            case Domain::Editor::EnemyType::LittleDevil: return U"リトルデビル";
            case Domain::Editor::EnemyType::Spider: return U"スパイダー";
            case Domain::Editor::EnemyType::Ribbon: return U"リボン";
            case Domain::Editor::EnemyType::Eye: return U"アイ";
            case Domain::Editor::EnemyType::Clown: return U"クラウン";
            case Domain::Editor::EnemyType::Boss1_3: return U"ボス1-3";
            default: return U"不明";
            }
        }
        
        ColorF getEnemyColor(Domain::Editor::EnemyType type) const
        {
            switch (type)
            {
            case Domain::Editor::EnemyType::LittleDevil: return ColorF{1.0, 0.5, 0.5};
            case Domain::Editor::EnemyType::Spider: return ColorF{0.5, 0.3, 0.8};
            case Domain::Editor::EnemyType::Ribbon: return ColorF{1.0, 0.7, 0.8};
            case Domain::Editor::EnemyType::Eye: return ColorF{0.3, 0.8, 1.0};
            case Domain::Editor::EnemyType::Clown: return ColorF{0.9, 0.9, 0.3};
            case Domain::Editor::EnemyType::Boss1_3: return ColorF{1.0, 0.0, 0.0};
            default: return Palette::Gray;
            }
        }
    };
    
    inline void EnemyEditorRenderer::drawEnemy(const Domain::Editor::EnemyEditorObjectNew& enemy, bool isSelected) const
    {
        const ColorF color = getEnemyColor(enemy.data.type);
        const double radius = enemy.data.isBoss() ? 50.0 : 30.0;
        
        Circle{enemy.data.position, radius}.draw(color.withAlpha(0.6));
        Circle{enemy.data.position, radius}.drawFrame(3.0, isSelected ? ColorF(Palette::Yellow) : color);
        
        this->m_smallFont(getEnemyTypeName(enemy.data.type))
            .drawAt(enemy.data.position.movedBy(0, radius + 15), Palette::White);
    }
    
    inline void EnemyEditorRenderer::drawPatrolRange(const Domain::Editor::EnemyObject& enemy) const
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
