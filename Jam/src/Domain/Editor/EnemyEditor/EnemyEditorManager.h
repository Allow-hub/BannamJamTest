// ========================================
// EnemyEditorManager.h（リファクタリング版 - Modify改善）
// ========================================
#pragma once
#include "../Base/EditorManagerBase.h"
#include "EnemyEditorTypes.h"

namespace Jam::Domain::Editor
{
    // 敵用のエディタオブジェクト（新基底クラス対応）
    struct EnemyEditorObjectNew : EditorObjectBase<EnemyObject>
    {
        using DataType = EnemyObject;
        
        EnemyEditorObjectNew() = default;
        explicit EnemyEditorObjectNew(const EnemyObject& obj)
            : EditorObjectBase<EnemyObject>(obj) {}
    };
    
    // 敵用のコマンド（Modify改善版）
    struct EnemyEditorCommandNew : EditorCommandBase<EnemyEditorObjectNew>
    {
        // 基底クラスのoldData/newDataを使用
    };
    
    // 敵エディタマネージャ
    class EnemyEditorManager : public EditorManagerBase<EnemyEditorObjectNew, EnemyEditorCommandNew>
    {
    private:
        using Base = EditorManagerBase<EnemyEditorObjectNew, EnemyEditorCommandNew>;
        
    public:
        // ===== オブジェクト操作 =====
        
        size_t addObject(const EnemyObject& enemy) override
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
        
        void removeObject(size_t id) override
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
        
        void modifyObject(size_t id, const EnemyObject& newEnemy)
        {
            if (auto* obj = findObjectById(id)) {
                if (!m_isExecutingCommand) {
                    EnemyEditorCommandNew cmd;
                    cmd.type = EnemyEditorCommandNew::Type::Modify;
                    cmd.object = *obj;
                    cmd.oldData = obj->data;  // 改善：旧データを保存
                    cmd.newData = newEnemy;   // 改善：新データを保存
                    addToHistory(cmd);
                }
                
                obj->data = newEnemy;
            }
        }
        
        // ===== 検索 =====
        
        Optional<size_t> findObjectAt(const Vec2& pos) const
        {
            constexpr double clickRadius = 20.0;
            
            // 逆順で検索（上に重なっているものを優先）
            for (auto it = m_objects.rbegin(); it != m_objects.rend(); ++it) {
                if (pos.distanceFrom(it->data.position) < clickRadius) {
                    return it->id;
                }
            }
            return none;
        }
        
        // 新しい敵が既存の敵と重なっているかチェック
        bool hasOverlappingEnemy(const Vec2& pos, double minDistance = 50.0) const
        {
            for (const auto& enemy : m_objects) {
                if (pos.distanceFrom(enemy.data.position) < minDistance) {
                    return true;
                }
            }
            return false;
        }
        
        // ===== ファイルI/O =====
        
        void saveToJSON(const FilePath& path) const override
        {
            Array<JSON> enemiesArray;
            
            for (const auto& enemy : m_objects) {
                enemiesArray.push_back(enemyObjectToJSON(enemy.data));
            }
            
            JSON json = enemiesArray;
            json.save(path);
        }
        
        void loadFromJSON(const FilePath& path) override
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
        
    protected:
        // ===== コマンド実行（Modify改善版） =====
        
        void executeUndoCommand(const EnemyEditorCommandNew& cmd) override
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
                // 改善：oldDataを使用して元に戻す
                if (cmd.object.id && cmd.oldData) {
                    if (auto* obj = findObjectById(*cmd.object.id)) {
                        obj->data = *cmd.oldData;
                    }
                }
                break;
            }
        }
        
        void executeRedoCommand(const EnemyEditorCommandNew& cmd) override
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
                // 改善：newDataを使用してやり直す
                if (cmd.object.id && cmd.newData) {
                    if (auto* obj = findObjectById(*cmd.object.id)) {
                        obj->data = *cmd.newData;
                    }
                }
                break;
            }
        }
        
    private:
        // ===== JSON変換 =====
        
        JSON enemyObjectToJSON(const EnemyObject& enemy) const
        {
            JSON json;
            
            json[U"type"] = enemyTypeToString(enemy.type);
            
            JSON posJson;
            posJson[U"x"] = static_cast<int>(enemy.position.x);
            posJson[U"y"] = static_cast<int>(enemy.position.y);
            json[U"position"] = posJson;
            
            // ボス以外はAI設定を追加
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
                
                // Chase AI（対応する敵のみ）
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
        
        Optional<EnemyObject> jsonToEnemyObject(const JSON& json) const
        {
            EnemyObject enemy;
            
            // type
            if (!json.hasElement(U"type")) return none;
            enemy.type = stringToEnemyType(json[U"type"].getString());
            
            // position
            if (json.hasElement(U"position")) {
                enemy.position.x = json[U"position"][U"x"].get<double>();
                enemy.position.y = json[U"position"][U"y"].get<double>();
            }
            
            // AI設定（ボス以外）
            if (!enemy.isBoss() && json.hasElement(U"extra")) {
                const auto& aiJson = json[U"extra"][U"ai"];
                
                // Patrol
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
                
                // Chase
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
    };
}