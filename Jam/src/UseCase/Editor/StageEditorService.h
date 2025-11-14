#pragma once
#include "../../Domain/Editor/StageEditorManager.h"

namespace Jam::UseCase::Editor
{
    class StageEditorService
    {
    private:
        Domain::Editor::StageEditorManager m_stageManager;
        Domain::Editor::StageEditorSettings m_settings;
        Domain::Editor::StageEditorMode m_mode = Domain::Editor::StageEditorMode::Place;
        
        Camera2D m_camera{Vec2{0, 0}, 1.0};
        
        Optional<Vec2> m_placementStart;
        Domain::Stage::StageType m_currentStageType = Domain::Stage::StageType::Normal;
        Domain::Stage::GroundSide m_currentGroundSide = Domain::Stage::GroundSide::All;
        String m_currentTextureKey = U"default";
        Domain::Editor::PlacementOrientation m_placementOrientation = Domain::Editor::PlacementOrientation::Horizontal;
        
        Domain::Stage::MovementType m_currentMovementType = Domain::Stage::MovementType::Horizontal;
        double m_currentMovementDistance = 200.0;
        double m_currentMovementSpeed = 100.0;
        bool m_currentLoopMovement = true;
        
        double m_currentDamageAmount = 10.0;
        String m_currentMetadata = U"普通の床";
        
        bool m_isTestMode = false;
        
    public:
        StageEditorService();
        
        void setMode(Domain::Editor::StageEditorMode mode) { m_mode = mode; }
        Domain::Editor::StageEditorMode getMode() const { return m_mode; }
        
        void updateCamera();
        const Camera2D& getCamera() const { return m_camera; }
        Vec2 screenToWorld(const Vec2& screenPos) const;
        Vec2 snapToGrid(const Vec2& pos) const;
        
        void setCurrentStageType(Domain::Stage::StageType type) 
        { 
            m_currentStageType = type;
            
            switch (type)
            {
            case Domain::Stage::StageType::Normal:
            case Domain::Stage::StageType::MovingPlatform:
                m_currentGroundSide = Domain::Stage::GroundSide::All;
                break;
            case Domain::Stage::StageType::OneWayPlatform:
                m_currentGroundSide = Domain::Stage::GroundSide::Up;
                break;
            case Domain::Stage::StageType::DamagePlatform:
            case Domain::Stage::StageType::MovingDamagePlatform:
                m_currentGroundSide = Domain::Stage::GroundSide::None;
                break;
            }
            
            switch (type)
            {
            case Domain::Stage::StageType::Normal:
                m_currentMetadata = U"普通の床";
                break;
            case Domain::Stage::StageType::MovingPlatform:
                m_currentMetadata = U"動く床";
                break;
            case Domain::Stage::StageType::OneWayPlatform:
                m_currentMetadata = U"すり抜け床";
                break;
            case Domain::Stage::StageType::DamagePlatform:
                m_currentMetadata = U"ダメージ床";
                break;
            case Domain::Stage::StageType::MovingDamagePlatform:
                m_currentMetadata = U"動くダメージ床";
                break;
            }
        }
        void setCurrentGroundSide(Domain::Stage::GroundSide side) { m_currentGroundSide = side; }
        Domain::Stage::StageType getCurrentStageType() const { return m_currentStageType; }
        Domain::Stage::GroundSide getCurrentGroundSide() const { return m_currentGroundSide; }
        
        void setMovementType(Domain::Stage::MovementType type) { m_currentMovementType = type; }
        void setMovementDistance(double distance) { m_currentMovementDistance = distance; }
        void setMovementSpeed(double speed) { m_currentMovementSpeed = speed; }
        void setLoopMovement(bool loop) { m_currentLoopMovement = loop; }
        void setDamageAmount(double amount) { m_currentDamageAmount = amount; }
        void setMetadata(const String& metadata) { m_currentMetadata = metadata; }
        Domain::Stage::MovementType getMovementType() const { return m_currentMovementType; }
        double getMovementDistance() const { return m_currentMovementDistance; }
        double getMovementSpeed() const { return m_currentMovementSpeed; }
        bool getLoopMovement() const { return m_currentLoopMovement; }
        double getDamageAmount() const { return m_currentDamageAmount; }
        const String& getMetadata() const { return m_currentMetadata; }
        
        void setPlacementOrientation(Domain::Editor::PlacementOrientation orientation) { m_placementOrientation = orientation; }
        Domain::Editor::PlacementOrientation getPlacementOrientation() const { return m_placementOrientation; }
        void togglePlacementOrientation() 
        {
            m_placementOrientation = (m_placementOrientation == Domain::Editor::PlacementOrientation::Horizontal) 
                ? Domain::Editor::PlacementOrientation::Vertical 
                : Domain::Editor::PlacementOrientation::Horizontal;
        }
        
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