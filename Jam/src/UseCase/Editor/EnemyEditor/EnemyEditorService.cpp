#include "EnemyEditorService.h"

namespace Jam::UseCase::Editor
{
    EnemyEditorService::EnemyEditorService()
    {
    }

    void EnemyEditorService::setEnemyType(Domain::Editor::EnemyType type)
    {
        m_state.enemyType = type;
    }

    void EnemyEditorService::handlePlacement(const Vec2& mousePos)
    {
        if (MouseL.down())
        {
            auto enemy = createEnemyFromCurrent(mousePos);
            m_enemyManager.addEnemy(enemy);
        }
    }

    void EnemyEditorService::handleSelection(const Vec2& mousePos)
    {
        auto id = m_enemyManager.findEnemyAt(mousePos);
        
        bool isAdditiveSelect = KeyControl.pressed() || KeyShift.pressed();
        
        if (id)
        {
            if (KeyControl.pressed() && m_enemyManager.getSelectedIds().contains(*id))
            {
                m_enemyManager.deselectEnemy(*id);
            }
            else
            {
                m_enemyManager.selectEnemy(*id, isAdditiveSelect);
            }
        }
        else
        {
            if (!isAdditiveSelect)
            {
                m_enemyManager.clearSelection();
            }
        }
    }

    void EnemyEditorService::handleDeletion(const Vec2& mousePos)
    {
        auto id = m_enemyManager.findEnemyAt(mousePos);
        if (id)
        {
            m_enemyManager.removeEnemy(*id);
        }
    }

    Domain::Editor::EnemyObject EnemyEditorService::createEnemyFromCurrent(const Vec2& position) const
    {
        Domain::Editor::EnemyObject enemy;
        enemy.type = m_state.enemyType;
        enemy.position = position;
        
        // ボス以外はAI設定を追加
        if (!enemy.isBoss())
        {
            // 巡回ポイントを初期位置からの相対座標で設定
            Domain::Editor::PatrolPoint point1, point2;
            point1.position = Vec2{m_state.patrolDistance, 0};
            point2.position = Vec2{-m_state.patrolDistance, 0};
            
            enemy.patrol.patrolPoints.push_back(point1);
            enemy.patrol.patrolPoints.push_back(point2);
            enemy.patrol.loop = true;
            enemy.patrol.waitTime = m_state.patrolWaitTime;
            enemy.patrol.foundDistance = m_state.foundDistance;
            
            // 追跡AIを持つ敵の場合
            if (enemy.hasChaseAI())
            {
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
