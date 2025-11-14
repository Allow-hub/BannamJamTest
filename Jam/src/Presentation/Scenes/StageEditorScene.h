#pragma once
#include <Siv3D.hpp>
#include "../Editor/StageEditorRenderer.h"
#include "../../UseCase/Editor/StageEditorService.h"

namespace Jam::Presentation::Scenes
{
    using App = SceneManager<String>;
    
    class StageEditorScene : public App::Scene
    {
    private:
        UseCase::Editor::StageEditorService m_editorService;
        Editor::StageEditorRenderer m_renderer;
        
    public:
        StageEditorScene(const InitData& init)
            : IScene{ init }
        {
            m_renderer.init(&m_editorService);
            m_renderer.setEditableService(&m_editorService);
        }
        
        void update() override
        {
            if (KeyEscape.down())
            {
                System::Exit();
            }
            
            if (KeyControl.pressed() && KeyP.down())
            {
                m_editorService.saveStage(U"Assets/Stage/stage_edited.json");
                
                auto& core = Jam::Foundation::CoreManager::Instance();
                core.stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;
                
                changeScene(ToSceneString(SceneName::InGame));
                return;
            }
            
            m_editorService.updateCamera();
            
            if (Key1.down()) m_editorService.setMode(Domain::Editor::StageEditorMode::Select);
            if (Key2.down()) m_editorService.setMode(Domain::Editor::StageEditorMode::Place);
            if (Key3.down()) m_editorService.setMode(Domain::Editor::StageEditorMode::Delete);
            
            m_renderer.drawGUIPanel();
            
            if (m_editorService.isTestMode())
            {
                m_editorService.updateTest();
                return;
            }
            
            bool hasMouseInput = MouseL.down() || MouseL.pressed() || MouseL.up() || MouseR.down();
            
            const int guiPanelX = Scene::Width() - 300;
            const bool isMouseOverGUI = Cursor::Pos().x >= guiPanelX;
            
            if (hasMouseInput && !isMouseOverGUI)
            {
                Vec2 mousePos = m_editorService.screenToWorld(Cursor::Pos());
                
                if (KeyShift.pressed())
                {
                    m_editorService.setPlacementOrientation(Domain::Editor::PlacementOrientation::Horizontal);
                }
                else if (KeyAlt.pressed())
                {
                    m_editorService.setPlacementOrientation(Domain::Editor::PlacementOrientation::Vertical);
                }
                
                switch (m_editorService.getMode())
                {
                case Domain::Editor::StageEditorMode::Place:
                    if (MouseL.down() || MouseL.pressed())
                    {
                        m_editorService.handlePlacement(mousePos);
                    }
                    break;
                    
                case Domain::Editor::StageEditorMode::Select:
                    if (MouseL.down())
                    {
                        m_editorService.handleSelection(mousePos);
                    }
                    break;
                    
                case Domain::Editor::StageEditorMode::Delete:
                    if (MouseL.down() || MouseL.pressed())
                    {
                        m_editorService.handleDeletion(mousePos);
                    }
                    break;
                }
            }
            
            if (KeyControl.pressed() && KeyZ.down())
            {
                m_editorService.undo();
            }
            if (KeyControl.pressed() && KeyY.down())
            {
                m_editorService.redo();
            }
            
            if (KeyControl.pressed() && KeyS.down())
            {
                m_editorService.saveStage(U"Assets/Stage/stage_edited.json");
            }
            if (KeyControl.pressed() && KeyO.down())
            {
                m_editorService.loadStage(U"Assets/Stage/stage_edited.json");
            }
        }
        
        void draw() const override
        {
            m_renderer.draw();
        }
    };
}
