#pragma once
#include "../../../Domain/Editor/StageEditor/StageEditorManager.h"

namespace Jam::UseCase::Editor
{
    struct EditorState
    {
        Domain::Stage::StageType stageType = Domain::Stage::StageType::Normal;
        Domain::Stage::GroundSide groundSide = Domain::Stage::GroundSide::All;
        Domain::Stage::MovementType movementType = Domain::Stage::MovementType::Horizontal;
        double movementDistance = 200.0;
        double movementSpeed = 100.0;
        bool loopMovement = true;
        double damageAmount = 10.0;
        String metadata = U"普通の床";
    };

    class StageEditorService
    {
    private:
        Domain::Editor::StageEditorManager m_stageManager;
        Domain::Editor::StageEditorSettings m_settings;
        Domain::Editor::StageEditorMode m_mode = Domain::Editor::StageEditorMode::Place;
        
        Camera2D m_camera{Vec2{0, 0}, 1.0};
        
        Optional<Vec2> m_dragStart;
        Optional<Vec2> m_currentDragPos;
        
        EditorState m_state;
        
    public:
        StageEditorService();
        
        void setMode(Domain::Editor::StageEditorMode mode) { m_mode = mode; }
        Domain::Editor::StageEditorMode getMode() const { return m_mode; }
        
        void updateCamera();
        const Camera2D& getCamera() const { return m_camera; }
        Vec2 screenToWorld(const Vec2& screenPos) const;
        Vec2 snapToGrid(const Vec2& pos) const;
        
        void setCurrentStageType(Domain::Stage::StageType type);
        Domain::Stage::StageType getCurrentStageType() const { return m_state.stageType; }
        Domain::Stage::GroundSide getCurrentGroundSide() const { return m_state.groundSide; }
        
        void setMovementType(Domain::Stage::MovementType type) { m_state.movementType = type; }
        void setMovementDistance(double distance);
        void setMovementSpeed(double speed);
        void setLoopMovement(bool loop) { m_state.loopMovement = loop; }
        void setDamageAmount(double amount);
        void setMetadata(const String& metadata) { m_state.metadata = metadata; }
        
        Domain::Stage::MovementType getMovementType() const { return m_state.movementType; }
        double getMovementDistance() const { return m_state.movementDistance; }
        double getMovementSpeed() const { return m_state.movementSpeed; }
        bool getLoopMovement() const { return m_state.loopMovement; }
        double getDamageAmount() const { return m_state.damageAmount; }
        const String& getMetadata() const { return m_state.metadata; }
        
        void handlePlacement(const Vec2& mousePos);
        void handleSelection(const Vec2& mousePos);
        void handleDeletion(const Vec2& mousePos);
        
        Optional<RectF> getDragRect() const;
        
        void undo() { m_stageManager.undo(); }
        void redo() { m_stageManager.redo(); }
        bool canUndo() const { return m_stageManager.canUndo(); }
        bool canRedo() const { return m_stageManager.canRedo(); }
        
        void saveStage(const FilePath& path) { m_stageManager.saveToJSON(path); }
        void loadStage(const FilePath& path) { m_stageManager.loadFromJSON(path); }
        void newStage() { m_stageManager.clear(); }
        
        const Domain::Editor::StageEditorManager& getStageManager() const { return m_stageManager; }
        const Domain::Editor::StageEditorSettings& getSettings() const { return m_settings; }
        Domain::Editor::StageEditorSettings& getSettings() { return m_settings; }
        
    private:
        Domain::Stage::StageObject createStageObjectFromCurrent(const RectF& rect) const;
        void updateGroundSideForType(Domain::Stage::StageType type);
        void updateMetadataForType(Domain::Stage::StageType type);
    };
}