#include "EnemyEditorManager.h"

namespace Jam::Domain::Editor
{
    size_t EnemyEditorManager::addObject(const EnemyObject& enemy)
    {
        EnemyEditorObjectNew editorEnemy(enemy);
        editorEnemy.id = getNextId();
        
        m_objects.push_back(editorEnemy);
        
        if (!m_isExecutingCommand) {
            EnemyEditorCommandNew cmd;
            cmd.type = EnemyEditorCommandNew::Type::Add;
            cmd.object = editorEnemy;
            addToHistory(cmd);
        }
        
        return *editorEnemy.id;
    }
    
    void EnemyEditorManager::removeObject(size_t id)
    {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
            if (it->id == id) {
                if (!m_isExecutingCommand) {
                    EnemyEditorCommandNew cmd;
                    cmd.type = EnemyEditorCommandNew::Type::Delete;
                    cmd.object = *it;
                    addToHistory(cmd);
                }
                
                m_objects.erase(it);
                m_selectedIds.erase(id);
                break;
            }
        }
    }
    
    void EnemyEditorManager::modifyObject(size_t id, const EnemyObject& newEnemy)
    {
        if (auto* obj = findObjectById(id)) {
            if (!m_isExecutingCommand) {
                EnemyEditorCommandNew cmd;
                cmd.type = EnemyEditorCommandNew::Type::Modify;
                cmd.object = *obj;
                cmd.oldData = obj->data;
                cmd.newData = newEnemy;
                addToHistory(cmd);
            }
            
            obj->data = newEnemy;
        }
    }
    
    Optional<size_t> EnemyEditorManager::findObjectAt(const Vec2& pos) const
    {
        constexpr double clickRadius = 20.0;
        
        for (auto it = m_objects.rbegin(); it != m_objects.rend(); ++it) {
            if (pos.distanceFrom(it->data.position) < clickRadius) {
                return it->id;
            }
        }
        return none;
    }
    
    bool EnemyEditorManager::hasOverlappingEnemy(const Vec2& pos, double minDistance) const
    {
        for (const auto& enemy : m_objects) {
            if (pos.distanceFrom(enemy.data.position) < minDistance) {
                return true;
            }
        }
        return false;
    }
    
    void EnemyEditorManager::saveToJSON(const FilePath& path) const
    {
        Array<JSON> enemiesArray;
        
        for (const auto& enemy : m_objects) {
            enemiesArray.push_back(enemyObjectToJSON(enemy.data));
        }
        
        JSON json = enemiesArray;
        json.save(path);
    }
    
    void EnemyEditorManager::loadFromJSON(const FilePath& path)
    {
        clear();
        
        if (!FileSystem::Exists(path)) return;
        
        const JSON json = JSON::Load(path);
        if (!json || !json.isArray()) return;
        
        m_isExecutingCommand = true;
        
        for (const auto& enemyJson : json.arrayView()) {
            if (auto enemy = jsonToEnemyObject(enemyJson)) {
                addObject(*enemy);
            }
        }
        
        m_isExecutingCommand = false;
    }
    
    void EnemyEditorManager::executeUndoCommand(const EnemyEditorCommandNew& cmd)
    {
        switch (cmd.type) {
        case EnemyEditorCommandNew::Type::Add:
            if (cmd.object.id) {
                removeObjectDirect(*cmd.object.id);
            }
            break;
            
        case EnemyEditorCommandNew::Type::Delete:
            m_objects.push_back(cmd.object);
            break;
            
        case EnemyEditorCommandNew::Type::Modify:
            if (cmd.object.id && cmd.oldData) {
                if (auto* obj = findObjectById(*cmd.object.id)) {
                    obj->data = *cmd.oldData;
                }
            }
            break;
        }
    }
    
    void EnemyEditorManager::executeRedoCommand(const EnemyEditorCommandNew& cmd)
    {
        switch (cmd.type) {
        case EnemyEditorCommandNew::Type::Add:
            m_objects.push_back(cmd.object);
            break;
            
        case EnemyEditorCommandNew::Type::Delete:
            if (cmd.object.id) {
                removeObjectDirect(*cmd.object.id);
            }
            break;
            
        case EnemyEditorCommandNew::Type::Modify:
            if (cmd.object.id && cmd.newData) {
                if (auto* obj = findObjectById(*cmd.object.id)) {
                    obj->data = *cmd.newData;
                }
            }
            break;
        }
    }
    
    JSON EnemyEditorManager::enemyObjectToJSON(const EnemyObject& enemy) const
    {
        JSON json;
        
        json[U"type"] = enemyTypeToString(enemy.type);
        
        JSON posJson;
        posJson[U"x"] = static_cast<int>(enemy.position.x);
        posJson[U"y"] = static_cast<int>(enemy.position.y);
        json[U"position"] = posJson;
        
        if (!enemy.isBoss()) {
            JSON extraJson;
            JSON aiJson;
            JSON patrolJson;
            
            Array<JSON> patrolPointsArray;
            for (const auto& point : enemy.patrol.patrolPoints) {
                JSON pointJson;
                pointJson[U"x"] = static_cast<int>(point.position.x);
                pointJson[U"y"] = static_cast<int>(point.position.y);
                patrolPointsArray.push_back(pointJson);
            }
            
            patrolJson[U"patrolPoints"] = patrolPointsArray;
            patrolJson[U"loop"] = enemy.patrol.loop;
            patrolJson[U"waitTime"] = enemy.patrol.waitTime;
            patrolJson[U"foundDistance"] = enemy.patrol.foundDistance;
            aiJson[U"patrol"] = patrolJson;
            
            if (enemy.chase) {
                JSON chaseJson;
                
                chaseJson[U"attackRange"] = enemy.chase->attackRange;
                chaseJson[U"loseRange"] = enemy.chase->loseRange;
                chaseJson[U"moveSpeedFactor"] = enemy.chase->moveSpeedFactor;
                aiJson[U"chase"] = chaseJson;
            }
            
            extraJson[U"ai"] = aiJson;
            json[U"extra"] = extraJson;
        }
        
        return json;
    }
    
    Optional<EnemyObject> EnemyEditorManager::jsonToEnemyObject(const JSON& json) const
    {
        EnemyObject enemy;
        
        if (!json.hasElement(U"type")) return none;
        enemy.type = stringToEnemyType(json[U"type"].getString());
        
        if (json.hasElement(U"position")) {
            enemy.position.x = json[U"position"][U"x"].get<double>();
            enemy.position.y = json[U"position"][U"y"].get<double>();
        }
        
        if (!enemy.isBoss() && json.hasElement(U"extra")) {
            const auto& aiJson = json[U"extra"][U"ai"];
            
            if (aiJson.hasElement(U"patrol")) {
                const auto& patrolJson = aiJson[U"patrol"];
                
                if (patrolJson.hasElement(U"patrolPoints")) {
                    for (const auto& pointJson : patrolJson[U"patrolPoints"].arrayView()) {
                        PatrolPoint point;
                        point.position.x = pointJson[U"x"].get<double>();
                        point.position.y = pointJson[U"y"].get<double>();
                        enemy.patrol.patrolPoints.push_back(point);
                    }
                }
                
                if (patrolJson.hasElement(U"loop")) {
                    enemy.patrol.loop = patrolJson[U"loop"].get<bool>();
                }
                if (patrolJson.hasElement(U"waitTime")) {
                    enemy.patrol.waitTime = patrolJson[U"waitTime"].get<double>();
                }
                if (patrolJson.hasElement(U"foundDistance")) {
                    enemy.patrol.foundDistance = patrolJson[U"foundDistance"].get<double>();
                }
            }
            
            if (enemy.hasChaseAI() && aiJson.hasElement(U"chase")) {
                ChaseAI chase;
                const auto& chaseJson = aiJson[U"chase"];
                
                if (chaseJson.hasElement(U"attackRange")) {
                    chase.attackRange = chaseJson[U"attackRange"].get<double>();
                }
                if (chaseJson.hasElement(U"loseRange")) {
                    chase.loseRange = chaseJson[U"loseRange"].get<double>();
                }
                if (chaseJson.hasElement(U"moveSpeedFactor")) {
                    chase.moveSpeedFactor = chaseJson[U"moveSpeedFactor"].get<double>();
                }
                
                enemy.chase = chase;
            }
        }
        
        return enemy;
    }
}
