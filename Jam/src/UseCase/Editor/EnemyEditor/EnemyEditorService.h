#pragma once
#include "../../../Domain/Editor/EnemyEditor/EnemyEditorManager.h"
#include "../../../Domain/Editor/StageEditor/StageEditorTypes.h"
#include "../../../Presentation/CameraManager.h"

namespace Jam::UseCase::Editor
{
    struct EnemyEditorState
    {
        Domain::Editor::EnemyType enemyType = Domain::Editor::EnemyType::LittleDevil;
        double patrolDistance = 300.0;
        double patrolWaitTime = 1.0;
        double foundDistance = 700.0;
        double attackRange = 600.0;
        double loseRange = 700.0;
        double moveSpeedFactor = 1.2;
    };

    class EnemyEditorService
    {
    private:
        Domain::Editor::EnemyEditorManager m_enemyManager;
        EnemyEditorState m_state;
        Presentation::CameraManager m_cameraManager;
        
        int m_patrolPointIndex = 0;
        Optional<Vec2> m_placingPosition;
        Array<Vec2> m_tempPatrolPoints;
        
    public:
        EnemyEditorService();
        
        void updateCamera() { m_cameraManager.update(Scene::DeltaTime()); }
        auto createCameraTransformer() const { return m_cameraManager.createTransformer(); }
        Vec2 screenToWorld(const Vec2& screenPos) const { return m_cameraManager.screenToWorld(screenPos); }
        
        void setEnemyType(Domain::Editor::EnemyType type);
        Domain::Editor::EnemyType getEnemyType() const { return m_state.enemyType; }
        
        void setPatrolDistance(double distance) { m_state.patrolDistance = distance; }
        void setPatrolWaitTime(double time) { m_state.patrolWaitTime = time; }
        void setFoundDistance(double distance) { m_state.foundDistance = distance; }
        void setAttackRange(double range) { m_state.attackRange = range; }
        void setLoseRange(double range) { m_state.loseRange = range; }
        void setMoveSpeedFactor(double factor) { m_state.moveSpeedFactor = factor; }
        
        double getPatrolDistance() const { return m_state.patrolDistance; }
        double getPatrolWaitTime() const { return m_state.patrolWaitTime; }
        double getFoundDistance() const { return m_state.foundDistance; }
        double getAttackRange() const { return m_state.attackRange; }
        double getLoseRange() const { return m_state.loseRange; }
        double getMoveSpeedFactor() const { return m_state.moveSpeedFactor; }
        
        void handlePlacement(const Vec2& mousePos);
        void handleSelection(const Vec2& mousePos);
        void handleDeletion(const Vec2& mousePos);
        
        void undo() { m_enemyManager.undo(); }
        void redo() { m_enemyManager.redo(); }
        bool canUndo() const { return m_enemyManager.canUndo(); }
        bool canRedo() const { return m_enemyManager.canRedo(); }
        
        void saveEnemies(const FilePath& path) { m_enemyManager.saveToJSON(path); }
        void loadEnemies(const FilePath& path) { m_enemyManager.loadFromJSON(path); }
        void newEnemies() { m_enemyManager.clear(); }
        
        const Domain::Editor::EnemyEditorManager& getEnemyManager() const { return m_enemyManager; }
        
        int getPatrolPointIndex() const { return m_patrolPointIndex; }
        const Optional<Vec2>& getPlacingPosition() const { return m_placingPosition; }
        const Array<Vec2>& getTempPatrolPoints() const { return m_tempPatrolPoints; }
        
    private:
        Domain::Editor::EnemyObject createEnemyFromCurrent(const Vec2& position) const;
    };
}
