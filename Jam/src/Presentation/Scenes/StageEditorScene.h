#pragma once
#include <Siv3D.hpp>
#include "TitleScene.h"
#include "../Editor/StageEditor/StageEditorRenderer.h"
#include "../Editor/EnemyEditor/EnemyEditorRenderer.h"
#include "../../UseCase/Editor/StageEditor/StageEditorService.h"
#include "../../UseCase/Editor/EnemyEditor/EnemyEditorService.h"
#include "../../Domain/Editor/EditorTypes.h"

namespace Jam::Presentation::Scenes
{
    using App = SceneManager<String>;
    
    class StageEditorScene : public App::Scene
    {
    private:
        Domain::Editor::EditorTarget m_editorTarget = Domain::Editor::EditorTarget::Stage;
        
        UseCase::Editor::StageEditorService m_stageEditorService;
        Editor::StageEditorRenderer m_stageRenderer;
        
        UseCase::Editor::EnemyEditorService m_enemyEditorService;
        Editor::EnemyEditorRenderer m_enemyRenderer;
        
    public:
        StageEditorScene(const InitData& init)
            : IScene{ init }
        {
            m_stageRenderer.init(&m_stageEditorService);
            m_enemyRenderer.init(&m_enemyEditorService);
        }
        
        void update() override
        {
            if (KeyEscape.down())
            {
                System::Exit();
            }
            
            // Editor Target切り替え (Tab or GUI Button)
            if (KeyTab.down())
            {
                m_editorTarget = (m_editorTarget == Domain::Editor::EditorTarget::Stage) 
                    ? Domain::Editor::EditorTarget::Enemy 
                    : Domain::Editor::EditorTarget::Stage;
            }
            
            // GUIボタンからの切り替え
            if (m_editorTarget == Domain::Editor::EditorTarget::Stage && m_stageRenderer.isEditorSwitchRequested())
            {
                m_editorTarget = Domain::Editor::EditorTarget::Enemy;
                m_stageRenderer.resetEditorSwitchRequest();
            }
            else if (m_editorTarget == Domain::Editor::EditorTarget::Enemy && m_enemyRenderer.isEditorSwitchRequested())
            {
                m_editorTarget = Domain::Editor::EditorTarget::Stage;
                m_enemyRenderer.resetEditorSwitchRequest();
            }
            
            if (m_editorTarget == Domain::Editor::EditorTarget::Stage)
            {
                updateStageEditor();
            }
            else
            {
                updateEnemyEditor();
            }
        }
        
        void updateStageEditor()
        {
            if (KeyControl.pressed() && KeyP.down())
            {
                m_stageEditorService.save(U"Assets/Stage/stage_edited.json");
                
                auto& core = Jam::Foundation::CoreManager::Instance();
                core.stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;
                
                changeScene(ToSceneString(SceneName::InGame));
                return;
            }
            
            m_stageEditorService.updateCamera();
            
            // Alt押下中はモード切り替えしない（Alt+Tabなどのシステムショートカット対策）
            if (!KeyAlt.pressed())
            {
                if (Key1.down()) m_stageEditorService.setMode(UseCase::Editor::EditorMode::Select);
                if (Key2.down()) m_stageEditorService.setMode(UseCase::Editor::EditorMode::Place);
                if (Key3.down()) m_stageEditorService.setMode(UseCase::Editor::EditorMode::Delete);
            }
            
            bool hasMouseInput = MouseL.down() || MouseL.pressed() || MouseL.up();
            
            const int guiPanelX = Scene::Width() - 300;
            const bool isMouseOverGUI = Cursor::Pos().x >= guiPanelX;
            
            if (hasMouseInput && !isMouseOverGUI)
            {
                Vec2 mousePos = m_stageEditorService.screenToWorld(Cursor::Pos());
                
                switch (m_stageEditorService.getMode())
                {
                case UseCase::Editor::EditorMode::Place:
                    if (MouseL.down() || MouseL.pressed() || MouseL.up())
                    {
                        m_stageEditorService.handlePlacement(mousePos);
                    }
                    break;
                    
                case UseCase::Editor::EditorMode::Select:
                    if (MouseL.down())
                    {
                        m_stageEditorService.handleSelection(mousePos);
                    }
                    break;
                case UseCase::Editor::EditorMode::Delete:
                    if (MouseL.down() || MouseL.pressed())
                    {
                        m_stageEditorService.handleDeletion(mousePos);
                    }
                    break;
                }
            }
            
            if (KeyControl.pressed() && KeyZ.down())
            {
                m_stageEditorService.undo();
            }
            if (KeyControl.pressed() && KeyY.down())
            {
                m_stageEditorService.redo();
            }
            
            // Deleteキーで選択中のオブジェクトを削除
            if (KeyDelete.down())
            {
                auto selectedIds = m_stageEditorService.getManager().getSelectedIds();
                for (auto selectedId : selectedIds)
                {
                    m_stageEditorService.getManager().removeObject(selectedId);
                }
            }
            
            if (KeyControl.pressed() && KeyS.down())
            {
                if (const auto path = Dialog::SaveFile({ FileFilter::JSON() }, U"Assets/Stage/"))
                {
                    m_stageEditorService.save(*path);
                }
            }
            if (KeyControl.pressed() && KeyO.down())
            {
                if (const auto path = Dialog::OpenFile({ FileFilter::JSON() }, U"Assets/Stage/"))
                {
                    m_stageEditorService.load(*path);
                }
            }
        }
        
        void updateEnemyEditor()
        {
            if (KeyControl.pressed() && KeyP.down())
            {
                m_stageEditorService.save(U"Assets/Stage/stage_edited.json");
                m_enemyEditorService.save(U"Assets/Enemy/enemy_edited.json");
                
                auto& core = Jam::Foundation::CoreManager::Instance();
                core.stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;
                
                changeScene(ToSceneString(SceneName::InGame));
                return;
            }
            
            m_enemyEditorService.updateCamera();
            
            // Alt押下中はモード切り替えしない（Alt+Tabなどのシステムショートカット対策）
            if (!KeyAlt.pressed())
            {
                if (Key1.down()) m_enemyEditorService.setMode(UseCase::Editor::EditorMode::Select);
                if (Key2.down()) m_enemyEditorService.setMode(UseCase::Editor::EditorMode::Place);
                if (Key3.down()) m_enemyEditorService.setMode(UseCase::Editor::EditorMode::Delete);
            }
            
            bool hasMouseInput = MouseL.down() || MouseL.pressed();
            
            const int guiPanelX = Scene::Width() - 300;
            const bool isMouseOverGUI = Cursor::Pos().x >= guiPanelX;
            
            if (hasMouseInput && !isMouseOverGUI)
            {
                Vec2 mousePos = m_enemyEditorService.screenToWorld(Cursor::Pos());
                
                switch (m_enemyEditorService.getMode())
                {
                case UseCase::Editor::EditorMode::Place:
                    if (MouseL.down())
                    {
                        m_enemyEditorService.handlePlacement(mousePos);
                    }
                    break;
                    
                case UseCase::Editor::EditorMode::Select:
                    if (MouseL.down())
                    {
                        m_enemyEditorService.handleSelection(mousePos);
                    }
                    break;
                    
                case UseCase::Editor::EditorMode::Delete:
                    if (MouseL.down() || MouseL.pressed())
                    {
                        m_enemyEditorService.handleDeletion(mousePos);
                    }
                    break;
                }
            }
            
            if (KeyControl.pressed() && KeyZ.down())
            {
                m_enemyEditorService.undo();
            }
            if (KeyControl.pressed() && KeyY.down())
            {
                m_enemyEditorService.redo();
            }
            
            // Deleteキーで選択中のオブジェクトを削除
            if (KeyDelete.down())
            {
                auto selectedIds = m_enemyEditorService.getManager().getSelectedIds();
                for (auto selectedId : selectedIds)
                {
                    m_enemyEditorService.getManager().removeObject(selectedId);
                }
            }
            
            if (KeyControl.pressed() && KeyS.down())
            {
                if (const auto path = Dialog::SaveFile({ FileFilter::JSON() }, U"Assets/Enemy/"))
                {
                    m_enemyEditorService.save(*path);
                }
            }
            if (KeyControl.pressed() && KeyO.down())
            {
                if (const auto path = Dialog::OpenFile({ FileFilter::JSON() }, U"Assets/Enemy/"))
                {
                    m_enemyEditorService.load(*path);
                }
            }
        }
        
        void draw() const override
        {
            Scene::SetBackground(ColorF{0.1, 0.15, 0.25});
            
            // アクティブなエディタのビューとGUIパネルを描画
            if (m_editorTarget == Domain::Editor::EditorTarget::Stage)
            {
                m_stageRenderer.drawView();
                m_stageRenderer.drawGUIPanel();
            }
            else
            {
                // 敵エディタの場合、敵エディタのカメラでグリッド・ステージ・敵を描画
                {
                    const auto& camera = m_enemyEditorService.getCamera();
                    auto transformer = camera.createTransformer();
                    
                    // グリッド描画
                    drawGridForEnemyEditor(camera);
                    
                    // ステージを背景として描画（カメラ変換は既に適用済み）
                    drawStageBackground();
                    
                    // 敵を描画（カメラ変換は既に適用済み）
                    drawEnemies();
                }
                
                // GUIパネルは変換なしで描画
                m_enemyRenderer.drawGUIPanel();
            }
        }
        
    private:
        void drawGridForEnemyEditor(const Camera2D& camera) const
        {
            const int gridSize = m_enemyEditorService.getSettings().getGridSize();
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
        
        void drawStageBackground() const
        {
            const auto& objects = m_stageEditorService.getManager().getAllObjects();
            for (const auto& obj : objects)
            {
                m_stageRenderer.drawObject(obj, false);
            }
        }
        
        void drawEnemies() const
        {
            const Vec2 playerSpawnPos{0, 0};
            const double playerRadius = 20.0;
            Circle{playerSpawnPos, playerRadius}.draw(ColorF{0.0, 1.0, 1.0, 0.5});
            Circle{playerSpawnPos, playerRadius}.drawFrame(2.0, ColorF{0.0, 1.0, 1.0});
            
            for (const auto& enemy : m_enemyEditorService.getManager().getAllObjects())
            {
                m_enemyRenderer.drawEnemy(enemy, enemy.isSelected);
                if (!enemy.data.isBoss())
                {
                    m_enemyRenderer.drawPatrolRange(enemy.data);
                }
            }
        }
    };
}
