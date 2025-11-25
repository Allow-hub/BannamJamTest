#include "StageEditorManager.h"

namespace Jam::Domain::Editor
{
    size_t StageEditorManager::addObject(const Stage::StageObject& obj)
    {
        // ゴールオブジェクトを追加する場合、既存のゴールを削除
        if (obj.texturePath.includes(U"Goal.png") || obj.texturePath.includes(U"goal.png"))
        {
            removeExistingGoal();
        }
        
        StageEditorObject editorObj(obj);
        
        size_t newIndex = m_objects.size();
        m_objects.push_back(editorObj);
        
        if (!m_isExecutingCommand)
        {
            StageEditorCommand cmd;
            cmd.type = StageEditorCommand::Type::Add;
            cmd.object = editorObj;
            addToHistory(cmd);
        }
        
        return newIndex;
    }
    
    void StageEditorManager::removeObject(size_t index)
    {
        if (index < m_objects.size())
        {
            if (!m_isExecutingCommand)
            {
                StageEditorCommand cmd;
                cmd.type = StageEditorCommand::Type::Delete;
                cmd.object = m_objects[index];
                addToHistory(cmd);
            }
            
            removeObjectDirect(index);
        }
    }
    
    void StageEditorManager::moveObject(size_t index, const Vec2& newPos)
    {
        if (auto* obj = findObjectByIndex(index))
        {
            if (!m_isExecutingCommand)
            {
                StageEditorCommand cmd;
                cmd.type = StageEditorCommand::Type::Move;
                cmd.object = *obj;
                cmd.oldPos = obj->data.rect.pos;
                cmd.newPos = newPos;
                addToHistory(cmd);
            }
            
            obj->data.rect.setPos(newPos);
        }
    }
    
    void StageEditorManager::modifyObject(size_t index, const Stage::StageObject& newObj)
    {
        if (auto* obj = findObjectByIndex(index))
        {
            if (!m_isExecutingCommand)
            {
                StageEditorCommand cmd;
                cmd.type = StageEditorCommand::Type::Modify;
                cmd.object = *obj;
                cmd.oldData = obj->data;
                cmd.newData = newObj;
                addToHistory(cmd);
            }
            
            obj->data = newObj;
        }
    }
    
    void StageEditorManager::updateSelectedObjectsMovement(double distance, double speed, Stage::MovementType type)
    {
        for (size_t i = 0; i < m_objects.size(); ++i)
        {
            if (m_selectedIndices.contains(i))
            {
                if (m_objects[i].data.type == Stage::StageType::MovingPlatform ||
                    m_objects[i].data.type == Stage::StageType::MovingDamagePlatform)
                {
                    m_objects[i].data.movementDistance = distance;
                    m_objects[i].data.movementSpeed = speed;
                    m_objects[i].data.movementType = type;
                }
            }
        }
    }
    
    void StageEditorManager::updateSelectedObjectsDamage(double damage)
    {
        for (size_t i = 0; i < m_objects.size(); ++i)
        {
            if (m_selectedIndices.contains(i))
            {
                if (m_objects[i].data.type == Stage::StageType::DamagePlatform ||
                    m_objects[i].data.type == Stage::StageType::MovingDamagePlatform)
                {
                    m_objects[i].data.damageAmount = damage;
                }
            }
        }
    }
    
    Optional<size_t> StageEditorManager::findObjectAt(const Vec2& pos) const
    {
        for (size_t i = m_objects.size(); i-- > 0;)
        {
            if (m_objects[i].data.rect.contains(pos)) return i;
        }
        return none;
    }
    
    bool StageEditorManager::hasObjectAtExactPosition(const RectF& rect) const
    {
        for (const auto& obj : m_objects)
        {
            if (obj.data.rect.x == rect.x &&
                obj.data.rect.y == rect.y &&
                obj.data.rect.w == rect.w &&
                obj.data.rect.h == rect.h)
                return true;
        }
        return false;
    }
    
    bool StageEditorManager::hasOverlappingObject(const RectF& rect) const
    {
        for (const auto& obj : m_objects)
        {
            if (obj.data.rect.intersects(rect))  return true;
        }
        return false;
    }
    
    Optional<size_t> StageEditorManager::findGoalObject() const
    {
        for (size_t i = 0; i < m_objects.size(); ++i)
        {
            if (m_objects[i].data.texturePath.includes(U"Goal.png") || 
                m_objects[i].data.texturePath.includes(U"goal.png"))
                return i;
        }
        return none;
    }
    
    void StageEditorManager::removeExistingGoal()
    {
        if (auto goalIndex = findGoalObject())
        {
            // 履歴に追加せずに削除（ゴールは1つのみなので、旧ゴールの復元は不要）
            bool wasExecuting = m_isExecutingCommand;
            m_isExecutingCommand = true;
            removeObject(*goalIndex);
            m_isExecutingCommand = wasExecuting;
        }
    }
    
    void StageEditorManager::saveToJSON(const FilePath& path) const
    {
        Array<Stage::StageObject> mergedObjects = mergeAdjacentObjects();
        
        JSON json;
        Array<JSON> objectsArray;
        
        // ゴール位置を検出（Goal.pngテクスチャを持つオブジェクト）
        Vec2 detectedGoalPos = m_goalPosition;
        Vec2 detectedGoalSize = m_goalSize;
        bool goalFound = false;
        
        for (const auto& obj : mergedObjects)
        {
            // Goal.pngを持つオブジェクトをゴールとして認識
            if (obj.texturePath.includes(U"Goal.png") || obj.texturePath.includes(U"goal.png"))
            {
                detectedGoalPos = obj.rect.pos;
                detectedGoalSize = obj.rect.size;
                goalFound = true;
                // ゴールオブジェクトは通常のobjectsには含めない
                continue;
            }
            objectsArray.push_back(stageObjectToJSON(obj));
        }
        
        json[U"objects"] = objectsArray;
        
        // ゴール情報を保存
        JSON goalJson;
        goalJson[U"position"] = Array<double>{ detectedGoalPos.x, detectedGoalPos.y };
        goalJson[U"size"] = Array<double>{ detectedGoalSize.x, detectedGoalSize.y };
        json[U"goal"] = goalJson;
        
        // プレイヤースポーン位置を保存
        json[U"playerSpawn"] = Array<double>{ m_playerSpawnPosition.x, m_playerSpawnPosition.y };
        
        json.save(path);
    }
    
    void StageEditorManager::loadFromJSON(const FilePath& path)
    {
        clear();
        
        const JSON json = JSON::Load(path);
        if (!json || !json.hasElement(U"objects")) return;
        
        m_isExecutingCommand = true;
        
        for (const auto& objJson : json[U"objects"].arrayView())
        {
            if (auto obj = jsonToStageObject(objJson))
                addObject(*obj);
        }
        
        // ゴール情報を読み込み
        if (json.hasElement(U"goal"))
        {
            const auto& goalJson = json[U"goal"];
            if (goalJson.hasElement(U"position") && goalJson[U"position"].isArray())
            {
                const auto& posArray = goalJson[U"position"];
                if (posArray.size() >= 2)
                    m_goalPosition = Vec2(posArray[0].get<double>(), posArray[1].get<double>());
            }
            if (goalJson.hasElement(U"size") && goalJson[U"size"].isArray())
            {
                const auto& sizeArray = goalJson[U"size"];
                if (sizeArray.size() >= 2)
                    m_goalSize = Vec2(sizeArray[0].get<double>(), sizeArray[1].get<double>());
            }
            
            // ゴールオブジェクトを作成して追加（エディタ上で表示・編集可能にする）
            Stage::StageObject goalObj;
            goalObj.rect = RectF{m_goalPosition, m_goalSize};
            goalObj.type = Stage::StageType::Normal;
            goalObj.groundSide = Stage::GroundSide::All;
            goalObj.metadata = U"ゴール";
            goalObj.texturePath = U"Assets/Stage/Goal.png";
            addObject(goalObj);
        }
        
        // プレイヤースポーン位置を読み込み
        if (json.hasElement(U"playerSpawn") && json[U"playerSpawn"].isArray())
        {
            const auto& spawnArray = json[U"playerSpawn"];
            if (spawnArray.size() >= 2)
                m_playerSpawnPosition = Vec2(spawnArray[0].get<double>(), spawnArray[1].get<double>());
        }
        
        m_isExecutingCommand = false;
    }
    
    void StageEditorManager::executeUndoCommand(const StageEditorCommand& cmd)
    {
        switch (cmd.type)
        {
        case StageEditorCommand::Type::Add:
            // 最後に追加されたオブジェクトを削除
            if (!m_objects.isEmpty())
                m_objects.pop_back();
            break;
            
        case StageEditorCommand::Type::Delete:
            m_objects.push_back(cmd.object);
            break;
            
        case StageEditorCommand::Type::Move:
            if (cmd.oldPos)
            {
                for (auto& obj : m_objects)
                {
                    if (obj.data.rect.pos == *cmd.newPos)
                    {
                        obj.data.rect.setPos(*cmd.oldPos);
                        break;
                    }
                }
            }
            break;
            
        case StageEditorCommand::Type::Modify:
            if (cmd.oldData && cmd.newData)
            {
                for (auto& obj : m_objects)
                {
                    if (obj.data.rect == cmd.newData->rect)
                    {
                        obj.data = *cmd.oldData;
                        break;
                    }
                }
            }
            break;
        }
    }
    
    void StageEditorManager::executeRedoCommand(const StageEditorCommand& cmd)
    {
        switch (cmd.type)
        {
        case StageEditorCommand::Type::Add:
            m_objects.push_back(cmd.object);
            break;
            
        case StageEditorCommand::Type::Delete:
            // Delete時もインデックスが不明なため、データで検索
            for (size_t i = 0; i < m_objects.size(); ++i)
            {
                if (m_objects[i].data.rect == cmd.object.data.rect)
                {
                    removeObjectDirect(i);
                    break;
                }
            }
            break;
            
        case StageEditorCommand::Type::Move:
            if (cmd.newPos)
            {
                for (auto& obj : m_objects)
                {
                    if (cmd.oldPos && obj.data.rect.pos == *cmd.oldPos)
                    {
                        obj.data.rect.setPos(*cmd.newPos);
                        break;
                    }
                }
            }
            break;
            
        case StageEditorCommand::Type::Modify:
            if (cmd.oldData && cmd.newData)
            {
                for (auto& obj : m_objects)
                {
                    if (obj.data.rect == cmd.oldData->rect)
                    {
                        obj.data = *cmd.newData;
                        break;
                    }
                }
            }
            break;
        }
    }
    
    Array<Stage::StageObject> StageEditorManager::mergeAdjacentObjects() const
    {
        Array<Stage::StageObject> result;
        HashSet<size_t> merged;
        constexpr double epsilon = 0.5;
        
        for (size_t i = 0; i < m_objects.size(); ++i)
        {
            if (merged.contains(i)) continue;
            
            const auto& obj = m_objects[i].data;
            RectF mergedRect = obj.rect;
            merged.insert(i);
            
            bool foundMerge = true;
            while (foundMerge)
            {
                foundMerge = false;
                
                for (size_t j = 0; j < m_objects.size(); ++j)
                {
                    if (merged.contains(j)) continue;
                    
                    const auto& other = m_objects[j].data;
                    
                    if (obj.type != other.type ||
                        obj.groundSide != other.groundSide ||
                        obj.movementType != other.movementType ||
                        Math::Abs(obj.movementSpeed - other.movementSpeed) > epsilon ||
                        Math::Abs(obj.movementDistance - other.movementDistance) > epsilon ||
                        Math::Abs(obj.damageAmount - other.damageAmount) > epsilon)
                    {
                        continue;
                    }
                    
                    const RectF otherRect = other.rect;
                    
                    if (Math::Abs(mergedRect.y - otherRect.y) < epsilon && 
                        Math::Abs(mergedRect.h - otherRect.h) < epsilon)
                    {
                        if (Math::Abs((mergedRect.x + mergedRect.w) - otherRect.x) < epsilon)
                        {
                            mergedRect.w = otherRect.x + otherRect.w - mergedRect.x;
                            merged.insert(j);
                            foundMerge = true;
                        }
                        else if (Math::Abs((otherRect.x + otherRect.w) - mergedRect.x) < epsilon)
                        {
                            double rightEdge = mergedRect.x + mergedRect.w;
                            mergedRect.x = otherRect.x;
                            mergedRect.w = rightEdge - otherRect.x;
                            merged.insert(j);
                            foundMerge = true;
                        }
                    }
                    else if (Math::Abs(mergedRect.x - otherRect.x) < epsilon && 
                             Math::Abs(mergedRect.w - otherRect.w) < epsilon)
                    {
                        if (Math::Abs((mergedRect.y + mergedRect.h) - otherRect.y) < epsilon)
                        {
                            mergedRect.h = otherRect.y + otherRect.h - mergedRect.y;
                            merged.insert(j);
                            foundMerge = true;
                        }
                        else if (Math::Abs((otherRect.y + otherRect.h) - mergedRect.y) < epsilon)
                        {
                            double bottomEdge = mergedRect.y + mergedRect.h;
                            mergedRect.y = otherRect.y;
                            mergedRect.h = bottomEdge - otherRect.y;
                            merged.insert(j);
                            foundMerge = true;
                        }
                    }
                }
            }
            
            Stage::StageObject mergedObj = obj;
            mergedObj.rect = mergedRect;
            result.push_back(mergedObj);
        }
        
        return result;
    }
    
    JSON StageEditorManager::stageObjectToJSON(const Stage::StageObject& obj) const
    {
        JSON json;
        
        Array<int> rectArray = {
            static_cast<int>(obj.rect.x),
            static_cast<int>(obj.rect.y),
            static_cast<int>(obj.rect.w),
            static_cast<int>(obj.rect.h)
        };
        json[U"rect"] = rectArray;
        
        json[U"type"] = Stage::collisionTypeToString(obj.type);
        json[U"groundSide"] = Stage::groundSideToString(obj.groundSide);
        json[U"metadata"] = obj.metadata;
        
        // テクスチャ情報を保存（空でない場合のみ）
        if (!obj.texturePath.isEmpty())
        {
            json[U"texturePath"] = obj.texturePath;
        }
        
        if (obj.type == Stage::StageType::MovingPlatform ||
            obj.type == Stage::StageType::MovingDamagePlatform)
        {
            json[U"movementType"] = Stage::movementTypeToString(obj.movementType);
            json[U"movementSpeed"] = static_cast<int>(obj.movementSpeed);
            json[U"movementDistance"] = static_cast<int>(obj.movementDistance);
            json[U"loopMovement"] = obj.loopMovement;
        }
        
        if (obj.type == Stage::StageType::DamagePlatform ||
            obj.type == Stage::StageType::MovingDamagePlatform)
        {
            json[U"damageAmount"] = static_cast<int>(obj.damageAmount);
        }
        
        return json;
    }
    
    Optional<Stage::StageObject> StageEditorManager::jsonToStageObject(const JSON& json) const
    {
        Stage::StageObject obj;
        
        if (!json.hasElement(U"rect") || !json[U"rect"].isArray())
            return none;
        
        const auto& rectArray = json[U"rect"];
        if (rectArray.size() < 4) return none;
        
        obj.rect = RectF{
            rectArray[0].get<double>(),
            rectArray[1].get<double>(),
            rectArray[2].get<double>(),
            rectArray[3].get<double>()
        };
        
        if (json.hasElement(U"type"))
        {
            obj.type = Stage::stringToCollisionType(json[U"type"].getString());
        }
        if (json.hasElement(U"groundSide"))
        {
            obj.groundSide = Stage::stringToGroundSide(json[U"groundSide"].getString());
        }
        if (json.hasElement(U"metadata"))
        {
            obj.metadata = json[U"metadata"].getString();
        }
        
        // テクスチャ情報を読み込み
        if (json.hasElement(U"texturePath"))
        {
            obj.texturePath = json[U"texturePath"].getString();
        }
        else
        {
            // texturePathがない場合はデフォルト値を設定
            obj.texturePath = U"Assets/Stage/normal_stage.png";
        }
        
        if (json.hasElement(U"movementType"))
        {
            obj.movementType = Stage::stringToMovementType(json[U"movementType"].getString());
        }
        if (json.hasElement(U"movementSpeed"))
        {
            obj.movementSpeed = json[U"movementSpeed"].get<double>();
        }
        if (json.hasElement(U"movementDistance"))
        {
            obj.movementDistance = json[U"movementDistance"].get<double>();
        }
        if (json.hasElement(U"loopMovement"))
        {
            obj.loopMovement = json[U"loopMovement"].get<bool>();
        }
        
        if (json.hasElement(U"damageAmount"))
        {
            obj.damageAmount = json[U"damageAmount"].get<double>();
        }
        
        return obj;
    }
}