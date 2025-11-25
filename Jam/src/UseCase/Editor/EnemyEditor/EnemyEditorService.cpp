#include "EnemyEditorService.h"

namespace Jam::UseCase::Editor
{
    // 敵の配置処理
    void EnemyEditorService::handlePlacement(const Vec2& mousePos)
    {
        if (MouseL.down())
        {
            if (!m_manager.hasOverlappingEnemy(mousePos))
            {
                auto enemy = createEnemyFromCurrent(mousePos);
                m_manager.addObject(enemy);
            }
        }
    }
    
    // 敵の選択処理
    void EnemyEditorService::handleSelection(const Vec2& mousePos)
    {
        auto index = m_manager.findObjectAt(mousePos);
        bool isAdditiveSelect = KeyControl.pressed() || KeyShift.pressed();
        
        if (index)
        {
            if (KeyControl.pressed() && m_manager.getSelectedIndices().contains(*index))
                m_manager.deselectObject(*index);
            else
                m_manager.selectObject(*index, isAdditiveSelect);
        }
        else
        {
            if (!isAdditiveSelect)
                m_manager.clearSelection();
        }
    }
    
    // 敵の削除処理
    void EnemyEditorService::handleDeletion(const Vec2& mousePos)
    {
        if (auto id = m_manager.findObjectAt(mousePos))
            m_manager.removeObject(*id);
    }
    
    // 現在の設定から敵オブジェクトを生成
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
