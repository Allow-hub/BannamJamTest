#include "EnemyEditorService.h"

namespace Jam::UseCase::Editor
{
    void EnemyEditorService::handlePlacement(const Vec2& mousePos)
    {
        if (MouseL.down()) {
            if (!m_manager.hasOverlappingEnemy(mousePos)) {
                auto enemy = createEnemyFromCurrent(mousePos);
                m_manager.addObject(enemy);
            }
        }
    }
    
    void EnemyEditorService::handleSelection(const Vec2& mousePos)
    {
        auto id = m_manager.findObjectAt(mousePos);
        bool isAdditiveSelect = KeyControl.pressed() || KeyShift.pressed();
        
        if (id) {
            if (KeyControl.pressed() && m_manager.getSelectedIds().contains(*id)) {
                m_manager.deselectObject(*id);
            } else {
                m_manager.selectObject(*id, isAdditiveSelect);
            }
        } else {
            if (!isAdditiveSelect) {
                m_manager.clearSelection();
            }
        }
    }
    
    void EnemyEditorService::handleDeletion(const Vec2& mousePos)
    {
        if (auto id = m_manager.findObjectAt(mousePos)) {
            m_manager.removeObject(*id);
        }
    }
    
    Domain::Editor::EnemyObject EnemyEditorService::createEnemyFromCurrent(const Vec2& position) const
    {
        Domain::Editor::EnemyObject enemy;
        enemy.type = m_state.enemyType;
        enemy.position = position;
        
        if (!enemy.isBoss()) {
            // 巡回ポイントを敵の配置位置基準に設定（絶対座標）
            Domain::Editor::PatrolPoint point1, point2;
            point1.position = position + Vec2{m_state.patrolDistance, 0};
            point2.position = position + Vec2{-m_state.patrolDistance, 0};
            
            enemy.patrol.patrolPoints.push_back(point1);
            enemy.patrol.patrolPoints.push_back(point2);
            enemy.patrol.loop = true;
            enemy.patrol.waitTime = m_state.patrolWaitTime;
            enemy.patrol.foundDistance = m_state.foundDistance;
            
            if (enemy.hasChaseAI()) {
                Domain::Editor::ChaseAI chase;
                chase.attackRange = m_state.attackRange;
                chase.loseRange = m_state.loseRange;
                chase.moveSpeedFactor = m_state.moveSpeedFactor;
                enemy.chase = chase;
            }
        }
        
        return enemy;
    }
}
