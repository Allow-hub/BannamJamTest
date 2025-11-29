#pragma once
#include "../Base/EditorServiceBase.h"
#include "../../../Domain/Editor/EnemyEditor/EnemyEditorManager.h"
#include "../../../Domain/Editor/EditorSettings.h"

namespace Jam::UseCase::Editor
{
    struct EnemyEditorState
    {
        Domain::EnemyType enemyType = Domain::EnemyType::LittleDevil;
        double patrolDistance = 300.0;
        double patrolWaitTime = 1.0;
        double foundDistance = 700.0;
        double attackRange = 600.0;
        double loseRange = 700.0;
        double moveSpeedFactor = 1.2;
    };
    
    class EnemyEditorService : public EditorServiceBase<Domain::Editor::EnemyEditorManager>
    {
    private:
        Domain::Editor::EditorSettings m_settings;
        EnemyEditorState m_state;
        
    public:
        EnemyEditorService() = default;
        
        // ===== 設定アクセス =====
        const Domain::Editor::EditorSettings& getSettings() const { return m_settings; }
        Domain::Editor::EditorSettings& getSettings() { return m_settings; }
        
        // ===== 敵タイプ設定 =====
        void setEnemyType(Domain::EnemyType type) { m_state.enemyType = type; }
        Domain::EnemyType getEnemyType() const { return m_state.enemyType; }
        
        // ===== AI設定 =====
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
        
        // ===== 入力処理（オーバーライド） =====
        void handlePlacement(const Vec2& mousePos) override;
        void handleSelection(const Vec2& mousePos) override;
        void handleDeletion(const Vec2& mousePos) override;
        
    private:
        Domain::Editor::EnemyObject createEnemyFromCurrent(const Vec2& position) const;
    };
}