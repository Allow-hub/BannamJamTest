#pragma once
#include "../../Domain/Editor/StageEditorManager.h"

namespace Jam::UseCase::Editor
{
    class StageEditorService
    {
    private:
        Domain::Editor::StageEditorManager m_stageManager;
        Domain::Editor::StageEditorSettings m_settings;
        Domain::Editor::StageEditorMode m_mode = Domain::Editor::StageEditorMode::Select;
        
        Camera2D m_camera{Vec2{0, 0}, 1.0};
        
        Optional<Vec2> m_placementStart;
        Domain::Stage::StageType m_currentStageType = Domain::Stage::StageType::Normal;
        Domain::Stage::GroundSide m_currentGroundSide = Domain::Stage::GroundSide::All;
        String m_currentTextureKey = U"default";
        
        bool m_isTestMode = false;
        
    public:
        StageEditorService();
        
        void setMode(Domain::Editor::StageEditorMode mode) { m_mode = mode; }
        Domain::Editor::StageEditorMode getMode() const { return m_mode; }
        
        void updateCamera();
        const Camera2D& getCamera() const { return m_camera; }
        Vec2 screenToWorld(const Vec2& screenPos) const;
        Vec2 snapToGrid(const Vec2& pos) const;
        
        void setCurrentStageType(Domain::Stage::StageType type) { m_currentStageType = type; }
        void setCurrentGroundSide(Domain::Stage::GroundSide side) { m_currentGroundSide = side; }
        Domain::Stage::StageType getCurrentStageType() const { return m_currentStageType; }
        Domain::Stage::GroundSide getCurrentGroundSide() const { return m_currentGroundSide; }
        
        void handlePlacement(const Vec2& mousePos);
        void handleSelection(const Vec2& mousePos);
        void handleDeletion(const Vec2& mousePos);
        void handleMove(const Vec2& mousePos);
        
        void undo() { m_stageManager.undo(); }
        void redo() { m_stageManager.redo(); }
        bool canUndo() const { return m_stageManager.canUndo(); }
        bool canRedo() const { return m_stageManager.canRedo(); }
        
        void startTest();
        void stopTest();
        bool isTestMode() const { return m_isTestMode; }
        void updateTest();
        
        void saveStage(const FilePath& path) { m_stageManager.saveToJSON(path); }
        void loadStage(const FilePath& path) { m_stageManager.loadFromJSON(path); }
        void newStage() { m_stageManager.clear(); }
        
        const Domain::Editor::StageEditorManager& getStageManager() const { return m_stageManager; }
        const Domain::Editor::StageEditorSettings& getSettings() const { return m_settings; }
        Domain::Editor::StageEditorSettings& getSettings() { return m_settings; }
        
    private:
        Domain::Stage::StageObject createStageObjectFromCurrent(const RectF& rect) const;
    };
}