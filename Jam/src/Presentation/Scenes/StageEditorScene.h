#pragma once
#include <Siv3D.hpp>
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
                m_stageEditorService.saveStage(U"Assets/Stage/stage_edited.json");
                
                auto& core = Jam::Foundation::CoreManager::Instance();
                core.stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;
                
                changeScene(ToSceneString(SceneName::InGame));
                return;
            }
            
            m_stageEditorService.updateCamera();
            
            if (Key1.down()) m_stageEditorService.setMode(Domain::Editor::StageEditorMode::Select);
            if (Key2.down()) m_stageEditorService.setMode(Domain::Editor::StageEditorMode::Place);
            if (Key3.down()) m_stageEditorService.setMode(Domain::Editor::StageEditorMode::Delete);
            
            bool hasMouseInput = MouseL.down() || MouseL.pressed() || MouseL.up();
            
            const int guiPanelX = Scene::Width() - 300;
            const bool isMouseOverGUI = Cursor::Pos().x >= guiPanelX;
            
            if (hasMouseInput && !isMouseOverGUI)
            {
                Vec2 mousePos = m_stageEditorService.screenToWorld(Cursor::Pos());
                
                switch (m_stageEditorService.getMode())
                {
                case Domain::Editor::StageEditorMode::Place:
                    if (MouseL.down() || MouseL.pressed() || MouseL.up())
                    {
                        m_stageEditorService.handlePlacement(mousePos);
                    }
                    break;
                    
                case Domain::Editor::StageEditorMode::Select:
                    if (MouseL.down())
                    {
                        m_stageEditorService.handleSelection(mousePos);
                    }
                    break;
                case Domain::Editor::StageEditorMode::Delete:
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
            
            if (KeyControl.pressed() && KeyS.down())
            {
                if (const auto path = Dialog::SaveFile({ FileFilter::JSON() }, U"App/Assets/Stage/"))
                {
                    m_stageEditorService.saveStage(*path);
                }
            }
            if (KeyControl.pressed() && KeyO.down())
            {
                if (const auto path = Dialog::OpenFile({ FileFilter::JSON() }, U"App/Assets/Stage/"))
                {
                    m_stageEditorService.loadStage(*path);
                }
            }
        }
        
        void updateEnemyEditor()
        {
            if (KeyControl.pressed() && KeyP.down())
            {
                m_stageEditorService.saveStage(U"Assets/Stage/stage_edited.json");
                m_enemyEditorService.saveEnemies(U"Assets/Enemy/enemy_edited.json");
                
                auto& core = Jam::Foundation::CoreManager::Instance();
                core.stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;
                
                changeScene(ToSceneString(SceneName::InGame));
                return;
            }
            
            m_enemyEditorService.updateCamera();
            
            bool hasMouseInput = MouseL.down() || MouseL.pressed();
            
            const int guiPanelX = Scene::Width() - 300;
            const bool isMouseOverGUI = Cursor::Pos().x >= guiPanelX;
            
            if (hasMouseInput && !isMouseOverGUI)
            {
                Vec2 mousePos = m_enemyEditorService.screenToWorld(Cursor::Pos());
                
                if (MouseL.down())
                {
                    m_enemyEditorService.handlePlacement(mousePos);
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
            
            if (KeyControl.pressed() && KeyS.down())
            {
                if (const auto path = Dialog::SaveFile({ FileFilter::JSON() }, U"App/Assets/Enemy/"))
                {
                    m_enemyEditorService.saveEnemies(*path);
                }
            }
            if (KeyControl.pressed() && KeyO.down())
            {
                if (const auto path = Dialog::OpenFile({ FileFilter::JSON() }, U"App/Assets/Enemy/"))
                {
                    m_enemyEditorService.loadEnemies(*path);
                }
            }
        }
        
        void draw() const override
        {
            Scene::SetBackground(ColorF{0.1, 0.15, 0.25});
            
            // ステージビュー（グリッド、オブジェクト）は常に描画
            m_stageRenderer.drawStageView();
            
            // 敵も常に描画
            m_enemyRenderer.drawEnemies();
            
            // GUIパネルのみ切り替え
            if (m_editorTarget == Domain::Editor::EditorTarget::Stage)
            {
                m_stageRenderer.drawGUIPanel();
            }
            else
            {
                m_enemyRenderer.drawGUIPanel();
            }
        }
    };
}
