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
            
            bool hasMouseInput = MouseL.down() || MouseL.pressed() || MouseL.up();
            
            const int guiPanelX = Scene::Width() - 300;
            const bool isMouseOverGUI = Cursor::Pos().x >= guiPanelX;
            
            if (hasMouseInput && !isMouseOverGUI)
            {
                Vec2 mousePos = m_editorService.screenToWorld(Cursor::Pos());
                
                switch (m_editorService.getMode())
                {
                case Domain::Editor::StageEditorMode::Place:
                    if (MouseL.down() || MouseL.pressed() || MouseL.up())
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
                if (const auto path = Dialog::SaveFile({ FileFilter::JSON() }, U"App/Assets/Stage/"))
                {
                    m_editorService.saveStage(*path);
                }
            }
            if (KeyControl.pressed() && KeyO.down())
            {
                if (const auto path = Dialog::OpenFile({ FileFilter::JSON() }, U"App/Assets/Stage/"))
                {
                    m_editorService.loadStage(*path);
                }
            }
        }
        
        void draw() const override
        {
            m_renderer.draw();
        }
    };
}