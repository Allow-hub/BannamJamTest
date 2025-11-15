#include "EnemyEditorManager.h"

namespace Jam::Domain::Editor
{
    size_t EnemyEditorManager::addEnemy(const EnemyObject& enemy)
    {
        EnemyEditorObject editorEnemy;
        editorEnemy.enemyObject = enemy;
        editorEnemy.id = m_nextId++;
        
        m_enemies.push_back(editorEnemy);
        
        if (!m_isExecutingCommand)
        {
            EnemyEditorCommand cmd;
            cmd.type = EnemyEditorCommand::Type::Add;
            cmd.object = editorEnemy;
            addToHistory(cmd);
        }
        
        return *editorEnemy.id;
    }

    void EnemyEditorManager::removeEnemy(size_t id)
    {
        for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it)
        {
            if (it->id == id)
            {
                if (!m_isExecutingCommand)
                {
                    EnemyEditorCommand cmd;
                    cmd.type = EnemyEditorCommand::Type::Delete;
                    cmd.object = *it;
                    addToHistory(cmd);
                }
                
                m_enemies.erase(it);
                m_selectedIds.erase(id);
                break;
            }
        }
    }

    void EnemyEditorManager::removeEnemyDirect(size_t id)
    {
        for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it)
        {
            if (it->id == id)
            {
                m_enemies.erase(it);
                m_selectedIds.erase(id);
                break;
            }
        }
    }

    void EnemyEditorManager::modifyEnemy(size_t id, const EnemyObject& newEnemy)
    {
        for (auto& enemy : m_enemies)
        {
            if (enemy.id == id)
            {
                if (!m_isExecutingCommand)
                {
                    EnemyEditorCommand cmd;
                    cmd.type = EnemyEditorCommand::Type::Modify;
                    cmd.object = enemy;
                    addToHistory(cmd);
                }
                
                enemy.enemyObject = newEnemy;
                break;
            }
        }
    }

    void EnemyEditorManager::selectEnemy(size_t id, bool additive)
    {
        if (!additive)
        {
            m_selectedIds.clear();
        }
        
        m_selectedIds.insert(id);
        
        for (auto& enemy : m_enemies)
        {
            enemy.isSelected = m_selectedIds.contains(*enemy.id);
        }
    }

    void EnemyEditorManager::deselectEnemy(size_t id)
    {
        m_selectedIds.erase(id);
        
        for (auto& enemy : m_enemies)
        {
            if (enemy.id == id)
            {
                enemy.isSelected = false;
                break;
            }
        }
    }

    void EnemyEditorManager::clearSelection()
    {
        m_selectedIds.clear();
        
        for (auto& enemy : m_enemies)
        {
            enemy.isSelected = false;
        }
    }

    Array<const EnemyEditorObject*> EnemyEditorManager::getSelectedEnemies() const
    {
        Array<const EnemyEditorObject*> result;
        
        for (const auto& enemy : m_enemies)
        {
            if (m_selectedIds.contains(*enemy.id))
            {
                result.push_back(&enemy);
            }
        }
        
        return result;
    }

    Optional<size_t> EnemyEditorManager::findEnemyAt(const Vec2& pos) const
    {
        const double clickRadius = 20.0;
        
        for (auto it = m_enemies.rbegin(); it != m_enemies.rend(); ++it)
        {
            if (pos.distanceFrom(it->enemyObject.position) < clickRadius)
            {
                return it->id;
            }
        }
        return none;
    }

    void EnemyEditorManager::undo()
    {
        if (!canUndo()) return;
        
        m_historyIndex--;
        const auto& cmd = m_commandHistory[m_historyIndex];
        
        m_isExecutingCommand = true;
        
        switch (cmd.type)
        {
        case EnemyEditorCommand::Type::Add:
            removeEnemyDirect(*cmd.object.id);
            break;
        case EnemyEditorCommand::Type::Delete:
            {
                EnemyEditorObject obj = cmd.object;
                m_enemies.push_back(obj);
            }
            break;
        case EnemyEditorCommand::Type::Modify:
            for (auto& enemy : m_enemies)
            {
                if (enemy.id == cmd.object.id)
                {
                    enemy.enemyObject = cmd.object.enemyObject;
                    break;
                }
            }
            break;
        }
        
        m_isExecutingCommand = false;
    }

    void EnemyEditorManager::redo()
    {
        if (!canRedo()) return;
        
        const auto& cmd = m_commandHistory[m_historyIndex];
        m_historyIndex++;
        
        executeCommand(cmd);
    }

    void EnemyEditorManager::executeCommand(const EnemyEditorCommand& cmd)
    {
        m_isExecutingCommand = true;
        
        switch (cmd.type)
        {
        case EnemyEditorCommand::Type::Add:
            {
                EnemyEditorObject obj = cmd.object;
                m_enemies.push_back(obj);
            }
            break;
        case EnemyEditorCommand::Type::Delete:
            removeEnemyDirect(*cmd.object.id);
            break;
        case EnemyEditorCommand::Type::Modify:
            for (auto& enemy : m_enemies)
            {
                if (enemy.id == cmd.object.id)
                {
                    enemy.enemyObject = cmd.object.enemyObject;
                    break;
                }
            }
            break;
        }
        
        m_isExecutingCommand = false;
    }

    void EnemyEditorManager::addToHistory(const EnemyEditorCommand& cmd)
    {
        m_commandHistory.resize(m_historyIndex);
        m_commandHistory.push_back(cmd);
        m_historyIndex++;
    }

    void EnemyEditorManager::saveToJSON(const FilePath& path) const
    {
        String output = U"[\n";
        
        for (size_t i = 0; i < m_enemies.size(); ++i)
        {
            const auto& e = m_enemies[i].enemyObject;
            
            output += U"    {\n";
            output += U"        \"type\": \"" + enemyTypeToString(e.type) + U"\",\n";
            output += U"        \"position\": {\n";
            output += U"            \"x\": " + Format(static_cast<int>(e.position.x)) + U",\n";
            output += U"            \"y\": " + Format(static_cast<int>(e.position.y)) + U"\n";
            output += U"        }";
            
            // ボス以外はAI設定を追加
            if (!e.isBoss())
            {
                output += U",\n        \"extra\": {\n";
                output += U"            \"ai\": {\n";
                output += U"                \"patrol\": {\n";
                output += U"                    \"patrolPoints\": [\n";
                
                for (size_t j = 0; j < e.patrol.patrolPoints.size(); ++j)
                {
                    const auto& point = e.patrol.patrolPoints[j];
                    output += U"                        {\n";
                    output += U"                            \"x\": " + Format(static_cast<int>(point.position.x)) + U",\n";
                    output += U"                            \"y\": " + Format(static_cast<int>(point.position.y)) + U"\n";
                    output += U"                        }";
                    if (j < e.patrol.patrolPoints.size() - 1) output += U",";
                    output += U"\n";
                }
                
                output += U"                    ],\n";
                output += U"                    \"loop\": " + String(e.patrol.loop ? U"true" : U"false") + U",\n";
                output += U"                    \"waitTime\": " + Format(e.patrol.waitTime) + U",\n";
                output += U"                    \"foundDistance\": " + Format(e.patrol.foundDistance) + U"\n";
                output += U"                }";
                
                // chase設定（対応する敵のみ）
                if (e.chase)
                {
                    output += U",\n                \"chase\": {\n";
                    output += U"                    \"attackRange\": " + Format(e.chase->attackRange) + U",\n";
                    output += U"                    \"loseRange\": " + Format(e.chase->loseRange) + U",\n";
                    output += U"                    \"moveSpeedFactor\": " + Format(e.chase->moveSpeedFactor) + U"\n";
                    output += U"                }\n";
                }
                else
                {
                    output += U"\n";
                }
                
                output += U"            }\n";
                output += U"        }\n";
            }
            else
            {
                output += U"\n";
            }
            
            output += U"    }";
            if (i < m_enemies.size() - 1) output += U",";
            output += U"\n";
        }
        
        output += U"]\n";
        
        TextWriter writer{path};
        if (writer)
        {
            writer.write(output);
        }
    }

    void EnemyEditorManager::loadFromJSON(const FilePath& path)
    {
        clear();
        
        if (!FileSystem::Exists(path)) return;
        
        const JSON json = JSON::Load(path);
        if (!json || !json.isArray()) return;
        
        for (const auto& enemyJson : json.arrayView())
        {
            EnemyObject enemy;
            
            // type
            if (enemyJson.hasElement(U"type"))
            {
                enemy.type = stringToEnemyType(enemyJson[U"type"].getString());
            }
            
            // position
            if (enemyJson.hasElement(U"position"))
            {
                enemy.position.x = enemyJson[U"position"][U"x"].get<double>();
                enemy.position.y = enemyJson[U"position"][U"y"].get<double>();
            }
            
            // AI設定（ボス以外）
            if (!enemy.isBoss() && enemyJson.hasElement(U"extra"))
            {
                const auto& aiJson = enemyJson[U"extra"][U"ai"];
                
                // patrol
                if (aiJson.hasElement(U"patrol"))
                {
                    const auto& patrolJson = aiJson[U"patrol"];
                    
                    if (patrolJson.hasElement(U"patrolPoints"))
                    {
                        for (const auto& pointJson : patrolJson[U"patrolPoints"].arrayView())
                        {
                            PatrolPoint point;
                            point.position.x = pointJson[U"x"].get<double>();
                            point.position.y = pointJson[U"y"].get<double>();
                            enemy.patrol.patrolPoints.push_back(point);
                        }
                    }
                    
                    if (patrolJson.hasElement(U"loop"))
                        enemy.patrol.loop = patrolJson[U"loop"].get<bool>();
                    
                    if (patrolJson.hasElement(U"waitTime"))
                        enemy.patrol.waitTime = patrolJson[U"waitTime"].get<double>();
                    
                    if (patrolJson.hasElement(U"foundDistance"))
                        enemy.patrol.foundDistance = patrolJson[U"foundDistance"].get<double>();
                }
                
                // chase（対応する敵のみ）
                if (enemy.hasChaseAI() && aiJson.hasElement(U"chase"))
                {
                    ChaseAI chase;
                    const auto& chaseJson = aiJson[U"chase"];
                    
                    if (chaseJson.hasElement(U"attackRange"))
                        chase.attackRange = chaseJson[U"attackRange"].get<double>();
                    
                    if (chaseJson.hasElement(U"loseRange"))
                        chase.loseRange = chaseJson[U"loseRange"].get<double>();
                    
                    if (chaseJson.hasElement(U"moveSpeedFactor"))
                        chase.moveSpeedFactor = chaseJson[U"moveSpeedFactor"].get<double>();
                    
                    enemy.chase = chase;
                }
            }
            
            addEnemy(enemy);
        }
    }

    void EnemyEditorManager::clear()
    {
        m_enemies.clear();
        m_commandHistory.clear();
        m_historyIndex = 0;
        m_nextId = 0;
        m_selectedIds.clear();
    }
}
