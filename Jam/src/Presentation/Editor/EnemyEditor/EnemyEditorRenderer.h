#pragma once
#include "../Base/EditorRendererBase.h"
#include "../../../UseCase/Editor/EnemyEditor/EnemyEditorService.h"
#include "../../../Domain/Editor/StageEditor/StageEditorManager.h"
#include "../Utilities/EditorGridUtil.h"

namespace Jam::Presentation::Editor
{
    // 敵エディタレンダラー
    class EnemyEditorRenderer : public EditorRendererBase<UseCase::Editor::EnemyEditorService>
    {
    private:
        static constexpr double PLAYER_SPAWN_RADIUS = 20.0;
        static constexpr double PLAYER_SPAWN_CENTER_RADIUS = 5.0;
        static constexpr double FRAME_THICKNESS = 2.0;
        
        mutable bool m_isEnemyTypeDropdownOpen = false;
        mutable bool m_switchToStageEditor = false;
        const Domain::Editor::StageEditorManager* m_stageManager = nullptr;
        
        // テキスト入力フィールド
        mutable TextEditState m_patrolDistanceTextEdit;
        mutable TextEditState m_waitTimeTextEdit;
        mutable TextEditState m_foundDistanceTextEdit;
        mutable TextEditState m_attackRangeTextEdit;
        mutable TextEditState m_loseRangeTextEdit;
        mutable TextEditState m_speedFactorTextEdit;
        
    public:
        // StageEditorManagerへの参照を設定
        void setStageManager(const Domain::Editor::StageEditorManager* manager)
        {
            m_stageManager = manager;
        }
        
        void drawView() const override
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
                    m_switchToStageEditor = true;
                y += 45;
                
                y = this->drawCameraSettings(y);
                y = drawCurrentMode(y);
                y = drawEnemyTypeSelector(y);
                y = drawPatrolSettings(y);
                y = drawChaseSettings(y);
            }
        }
        
        bool isEditorSwitchRequested() const { return m_switchToStageEditor; }
        void resetEditorSwitchRequest() const { m_switchToStageEditor = false; }
        
        // テキスト入力中かどうかを判定
        bool isTextInputActive() const
        {
            return m_patrolDistanceTextEdit.active || m_waitTimeTextEdit.active ||
                   m_foundDistanceTextEdit.active || m_attackRangeTextEdit.active ||
                   m_loseRangeTextEdit.active || m_speedFactorTextEdit.active;
        }
        
        // StageEditorSceneから呼び出すためpublicに
        void drawEnemy(const Domain::Editor::EnemyEditorObject& enemy, bool isSelected) const;
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