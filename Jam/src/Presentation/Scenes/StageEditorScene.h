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
            
            if (KeyQ.down())
            {
                using StageType = Domain::Stage::StageType;
                auto currentType = m_editorService.getCurrentStageType();
                switch (currentType)
                {
                case StageType::Normal: m_editorService.setCurrentStageType(StageType::MovingPlatform); break;
                case StageType::MovingPlatform: m_editorService.setCurrentStageType(StageType::OneWayPlatform); break;
                case StageType::OneWayPlatform: m_editorService.setCurrentStageType(StageType::DamagePlatform); break;
                case StageType::DamagePlatform: m_editorService.setCurrentStageType(StageType::Normal); break;
                default: m_editorService.setCurrentStageType(StageType::Normal); break;
                }
            }
            
            if (KeyE.down())
            {
                using GroundSide = Domain::Stage::GroundSide;
                auto currentSide = m_editorService.getCurrentGroundSide();
                switch (currentSide)
                {
                case GroundSide::All: m_editorService.setCurrentGroundSide(GroundSide::Up); break;
                case GroundSide::Up: m_editorService.setCurrentGroundSide(GroundSide::Down); break;
                case GroundSide::Down: m_editorService.setCurrentGroundSide(GroundSide::Left); break;
                case GroundSide::Left: m_editorService.setCurrentGroundSide(GroundSide::Right); break;
                case GroundSide::Right: m_editorService.setCurrentGroundSide(GroundSide::None); break;
                case GroundSide::None: m_editorService.setCurrentGroundSide(GroundSide::All); break;
                default: m_editorService.setCurrentGroundSide(GroundSide::All); break;
                }
            }
            
            if (KeyZ.down())
            {
                using MovementType = Domain::Stage::MovementType;
                auto currentType = m_editorService.getMovementType();
                switch (currentType)
                {
                case MovementType::Horizontal: m_editorService.setMovementType(MovementType::Vertical); break;
                case MovementType::Vertical: m_editorService.setMovementType(MovementType::Circular); break;
                case MovementType::Circular: m_editorService.setMovementType(MovementType::Horizontal); break;
                default: m_editorService.setMovementType(MovementType::Horizontal); break;
                }
            }
            
            if (KeyX.down())
            {
                double currentDist = m_editorService.getMovementDistance();
                if (KeyShift.pressed())
                    m_editorService.setMovementDistance(Max(50.0, currentDist - 50.0));
                else
                    m_editorService.setMovementDistance(Min(1000.0, currentDist + 50.0));
            }
            
            if (KeyC.down())
            {
                double currentSpeed = m_editorService.getMovementSpeed();
                if (KeyShift.pressed())
                    m_editorService.setMovementSpeed(Max(10.0, currentSpeed - 10.0));
                else
                    m_editorService.setMovementSpeed(Min(500.0, currentSpeed + 10.0));
            }
            
            if (m_editorService.isTestMode())
            {
                m_editorService.updateTest();
                return;
            }
            
            bool hasMouseInput = MouseL.down() || MouseL.up() || MouseR.down();
            
            if (hasMouseInput)
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
                    if (MouseL.down())
                    {
                        m_editorService.handlePlacement(mousePos);
                    }
                    else if (MouseL.up() && MouseL.pressedDuration() > 0.1s)
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
                    if (MouseL.down())
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
