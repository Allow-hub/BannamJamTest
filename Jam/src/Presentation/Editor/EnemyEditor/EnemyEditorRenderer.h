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
        
    public:
        void drawView() const override
        {
            const auto& camera = this->m_service->getCamera();
            auto transformer = camera.createTransformer();
            
            // グリッド描画
            this->drawGrid(camera, this->m_service->getSettings().getGridSize());
            
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
        int drawCurrentMode(int y) const;
        int drawEnemyTypeSelector(int y) const;
        int drawPatrolSettings(int y) const;
        int drawChaseSettings(int y) const;
        String getEnemyTypeName(Domain::Editor::EnemyType type) const;
        ColorF getEnemyColor(Domain::Editor::EnemyType type) const;
    };
}
