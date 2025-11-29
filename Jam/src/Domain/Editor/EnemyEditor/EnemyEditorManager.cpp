#include "EnemyEditorManager.h"

namespace Jam::Domain::Editor
{
    // 敵オブジェクトを追加
    size_t EnemyEditorManager::addObject(const EnemyObject& enemy)
    {
        EnemyEditorObject editorEnemy(enemy);
        
        size_t newIndex = m_objects.size();
        m_objects.push_back(editorEnemy);
        
        if (!m_isExecutingCommand)
        {
            EnemyEditorCommand cmd;
            cmd.type = EnemyEditorCommand::Type::Add;
            cmd.object = editorEnemy;
            addToHistory(cmd);
        }
        
        return newIndex;
    }
    
    // 敵オブジェクトを削除
    void EnemyEditorManager::removeObject(size_t index)
    {
        if (index < m_objects.size())
        {
            if (!m_isExecutingCommand)
            {
                EnemyEditorCommand cmd;
                cmd.type = EnemyEditorCommand::Type::Delete;
                cmd.object = m_objects[index];
                addToHistory(cmd);
            }
            
            removeObjectDirect(index);
        }
    }
    
    // 敵オブジェクトの情報を変更
    void EnemyEditorManager::modifyObject(size_t index, const EnemyObject& newEnemy)
    {
        if (auto* obj = findObjectByIndex(index))
        {
            if (!m_isExecutingCommand)
            {
                EnemyEditorCommand cmd;
                cmd.type = EnemyEditorCommand::Type::Modify;
                cmd.object = *obj;
                cmd.oldData = obj->data;
                cmd.newData = newEnemy;
                addToHistory(cmd);
            }
            
            obj->data = newEnemy;
        }
    }
    
    // 指定座標にある敵オブジェクトを検索
    Optional<size_t> EnemyEditorManager::findObjectAt(const Vec2& pos) const
    {
        static constexpr double CLICK_RADIUS = 20.0;
        
        for (size_t i = m_objects.size(); i-- > 0;)
        {
            if (pos.distanceFrom(m_objects[i].data.position) < CLICK_RADIUS)
                return i;
        }
        return none;
    }
    
    // 指定位置に既存の敵が重なっているかチェック
    bool EnemyEditorManager::hasOverlappingEnemy(const Vec2& pos, double minDistance) const
    {
        for (const auto& enemy : m_objects)
        {
            if (pos.distanceFrom(enemy.data.position) < minDistance)
                return true;
        }
        return false;
    }
    
    // 敵配置データをJSONファイルに保存
    void EnemyEditorManager::saveToJSON(const FilePath& path) const
    {
        Array<JSON> enemiesArray;
        
        for (const auto& enemy : m_objects)
        {
            enemiesArray.push_back(enemyObjectToJSON(enemy.data));
        }
        
        JSON json = enemiesArray;
        json.save(path);
    }
    
    // JSONファイルから敵配置データを読み込み
    void EnemyEditorManager::loadFromJSON(const FilePath& path)
    {
        clear();
        
        if (!FileSystem::Exists(path)) return;
        
        const JSON json = JSON::Load(path);
        if (!json || !json.isArray()) return;
        
        m_isExecutingCommand = true;
        
        for (const auto& enemyJson : json.arrayView())
        {
            if (auto enemy = jsonToEnemyObject(enemyJson))
                addObject(*enemy);
        }
        
        m_isExecutingCommand = false;
    }
    
    // Undo操作を実行
    void EnemyEditorManager::executeUndoCommand(const EnemyEditorCommand& cmd)
    {
        switch (cmd.type)
        {
        case EnemyEditorCommand::Type::Add:
            if (!m_objects.isEmpty())
                m_objects.pop_back();
            break;
            
        case EnemyEditorCommand::Type::Delete:
            m_objects.push_back(cmd.object);
            break;
            
        case EnemyEditorCommand::Type::Modify:
            if (cmd.oldData)
            {
                for (auto& obj : m_objects)
                {
                    if (obj.data.position == cmd.object.data.position)
                    {
                        obj.data = *cmd.oldData;
                        break;
                    }
                }
            }
            break;
        }
    }
    
    // Redo操作を実行
    void EnemyEditorManager::executeRedoCommand(const EnemyEditorCommand& cmd)
    {
        switch (cmd.type)
        {
        case EnemyEditorCommand::Type::Add:
            m_objects.push_back(cmd.object);
            break;
            
        case EnemyEditorCommand::Type::Delete:
            for (size_t i = 0; i < m_objects.size(); ++i)
            {
                if (m_objects[i].data.position == cmd.object.data.position)
                {
                    removeObjectDirect(i);
                    break;
                }
            }
            break;
            
        case EnemyEditorCommand::Type::Modify:
            if (cmd.newData)
            {
                for (auto& obj : m_objects)
                {
                    if (obj.data.position == cmd.object.data.position)
                    {
                        obj.data = *cmd.newData;
                        break;
                    }
                }
            }
            break;
        }
    }
    
    // 敵オブジェクトをJSON形式に変換
    JSON EnemyEditorManager::enemyObjectToJSON(const EnemyObject& enemy) const
    {
        JSON json;
        
        json[U"type"] = Domain::enemyTypeToString(enemy.type);
        
        JSON posJson;
        posJson[U"x"] = static_cast<int>(enemy.position.x);
        posJson[U"y"] = static_cast<int>(enemy.position.y);
        json[U"position"] = posJson;
        
        if (!enemy.isBoss())
        {
            JSON extraJson;
            JSON aiJson;
            JSON patrolJson;
            
            Array<JSON> patrolPointsArray;
            for (const auto& point : enemy.patrol.patrolPoints)
            {
                JSON pointJson;
                pointJson[U"x"] = static_cast<int>(point.position.x);
                pointJson[U"y"] = static_cast<int>(point.position.y);
                patrolPointsArray.push_back(pointJson);
            }
            
            patrolJson[U"patrolPoints"] = patrolPointsArray;
            // AI設定値は直接AISettingsから取得
            Domain::Enemy::PatrolAISettings patrolSettings;
            patrolJson[U"loop"] = patrolSettings.loop;
            patrolJson[U"waitTime"] = patrolSettings.waitTime;
            patrolJson[U"foundDistance"] = patrolSettings.foundDistance;
            aiJson[U"patrol"] = patrolJson;
            
            if (enemy.hasChaseAI())
            {
                JSON chaseJson;
                
                // AI設定値は直接ChaseAISettingsから取得
                Domain::Enemy::ChaseAISettings chaseSettings;
                chaseJson[U"attackRange"] = chaseSettings.attackRange;
                chaseJson[U"loseRange"] = chaseSettings.loseRange;
                chaseJson[U"moveSpeedFactor"] = chaseSettings.moveSpeedFactor;
                aiJson[U"chase"] = chaseJson;
            }
            
            extraJson[U"ai"] = aiJson;
            json[U"extra"] = extraJson;
        }
        
        return json;
    }
    
    // JSONデータを敵オブジェクトに変換
    Optional<EnemyObject> EnemyEditorManager::jsonToEnemyObject(const JSON& json) const
    {
        EnemyObject enemy;
        
        if (!json.hasElement(U"type")) return none;
        enemy.type = Domain::stringToEnemyType(json[U"type"].getString());
        
        if (json.hasElement(U"position"))
        {
            enemy.position.x = json[U"position"][U"x"].get<double>();
            enemy.position.y = json[U"position"][U"y"].get<double>();
        }
        
        if (!enemy.isBoss() && json.hasElement(U"extra"))
        {
            const auto& aiJson = json[U"extra"][U"ai"];
            parsePatrolAI(enemy, aiJson);
        }
        
        return enemy;
    }
    
    // パトロールAI情報をJSONからパース
    void EnemyEditorManager::parsePatrolAI(EnemyObject& enemy, const JSON& aiJson) const
    {
        if (!aiJson.hasElement(U"patrol")) return;
        
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
        
    }
    

}