#include "StageEditorManager.h"

namespace Jam::Domain::Editor
{
    size_t StageEditorManager::addObject(const Stage::StageObject& obj)
    {
        StageEditorObjectNew editorObj(obj);
        editorObj.id = getNextId();
        
        m_objects.push_back(editorObj);
        
        if (!m_isExecutingCommand) {
            StageEditorCommandNew cmd;
            cmd.type = StageEditorCommandNew::Type::Add;
            cmd.object = editorObj;
            addToHistory(cmd);
        }
        
        return *editorObj.id;
    }
    
    void StageEditorManager::removeObject(size_t id)
    {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
            if (it->id == id) {
                if (!m_isExecutingCommand) {
                    StageEditorCommandNew cmd;
                    cmd.type = StageEditorCommandNew::Type::Delete;
                    cmd.object = *it;
                    addToHistory(cmd);
                }
                
                m_objects.erase(it);
                m_selectedIds.erase(id);
                break;
            }
        }
    }
    
    void StageEditorManager::moveObject(size_t id, const Vec2& newPos)
    {
        if (auto* obj = findObjectById(id)) {
            if (!m_isExecutingCommand) {
                StageEditorCommandNew cmd;
                cmd.type = StageEditorCommandNew::Type::Move;
                cmd.object = *obj;
                cmd.oldPos = obj->data.rect.pos;
                cmd.newPos = newPos;
                addToHistory(cmd);
            }
            
            obj->data.rect.setPos(newPos);
        }
    }
    
    void StageEditorManager::modifyObject(size_t id, const Stage::StageObject& newObj)
    {
        if (auto* obj = findObjectById(id)) {
            if (!m_isExecutingCommand) {
                StageEditorCommandNew cmd;
                cmd.type = StageEditorCommandNew::Type::Modify;
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
        for (auto& obj : m_objects) {
            if (obj.id && m_selectedIds.contains(*obj.id)) {
                if (obj.data.type == Stage::StageType::MovingPlatform ||
                    obj.data.type == Stage::StageType::MovingDamagePlatform) {
                    obj.data.movementDistance = distance;
                    obj.data.movementSpeed = speed;
                    obj.data.movementType = type;
                }
            }
        }
    }
    
    void StageEditorManager::updateSelectedObjectsDamage(double damage)
    {
        for (auto& obj : m_objects) {
            if (obj.id && m_selectedIds.contains(*obj.id)) {
                if (obj.data.type == Stage::StageType::DamagePlatform ||
                    obj.data.type == Stage::StageType::MovingDamagePlatform) {
                    obj.data.damageAmount = damage;
                }
            }
        }
    }
    
    Optional<size_t> StageEditorManager::findObjectAt(const Vec2& pos) const
    {
        for (auto it = m_objects.rbegin(); it != m_objects.rend(); ++it) {
            if (it->data.rect.contains(pos)) {
                return it->id;
            }
        }
        return none;
    }
    
    bool StageEditorManager::hasObjectAtExactPosition(const RectF& rect) const
    {
        for (const auto& obj : m_objects) {
            if (obj.data.rect.x == rect.x &&
                obj.data.rect.y == rect.y &&
                obj.data.rect.w == rect.w &&
                obj.data.rect.h == rect.h) {
                return true;
            }
        }
        return false;
    }
    
    bool StageEditorManager::hasOverlappingObject(const RectF& rect) const
    {
        for (const auto& obj : m_objects) {
            if (obj.data.rect.intersects(rect)) {
                return true;
            }
        }
        return false;
    }
    
    void StageEditorManager::saveToJSON(const FilePath& path) const
    {
        Array<Stage::StageObject> mergedObjects = mergeAdjacentObjects();
        
        JSON json;
        Array<JSON> objectsArray;
        
        for (const auto& obj : mergedObjects) {
            objectsArray.push_back(stageObjectToJSON(obj));
        }
        
        json[U"objects"] = objectsArray;
        json.save(path);
    }
    
    void StageEditorManager::loadFromJSON(const FilePath& path)
    {
        clear();
        
        const JSON json = JSON::Load(path);
        if (!json || !json.hasElement(U"objects")) return;
        
        m_isExecutingCommand = true;
        
        for (const auto& objJson : json[U"objects"].arrayView()) {
            if (auto obj = jsonToStageObject(objJson)) {
                addObject(*obj);
            }
        }
        
        m_isExecutingCommand = false;
    }
    
    void StageEditorManager::executeUndoCommand(const StageEditorCommandNew& cmd)
    {
        switch (cmd.type) {
        case StageEditorCommandNew::Type::Add:
            if (cmd.object.id) {
                removeObjectDirect(*cmd.object.id);
            }
            break;
            
        case StageEditorCommandNew::Type::Delete:
            m_objects.push_back(cmd.object);
            break;
            
        case StageEditorCommandNew::Type::Move:
            if (cmd.object.id && cmd.oldPos) {
                if (auto* obj = findObjectById(*cmd.object.id)) {
                    obj->data.rect.setPos(*cmd.oldPos);
                }
            }
            break;
            
        case StageEditorCommandNew::Type::Modify:
            if (cmd.object.id && cmd.oldData) {
                if (auto* obj = findObjectById(*cmd.object.id)) {
                    obj->data = *cmd.oldData;
                }
            }
            break;
        }
    }
    
    void StageEditorManager::executeRedoCommand(const StageEditorCommandNew& cmd)
    {
        switch (cmd.type) {
        case StageEditorCommandNew::Type::Add:
            m_objects.push_back(cmd.object);
            break;
            
        case StageEditorCommandNew::Type::Delete:
            if (cmd.object.id) {
                removeObjectDirect(*cmd.object.id);
            }
            break;
            
        case StageEditorCommandNew::Type::Move:
            if (cmd.object.id && cmd.newPos) {
                if (auto* obj = findObjectById(*cmd.object.id)) {
                    obj->data.rect.setPos(*cmd.newPos);
                }
            }
            break;
            
        case StageEditorCommandNew::Type::Modify:
            if (cmd.object.id && cmd.newData) {
                if (auto* obj = findObjectById(*cmd.object.id)) {
                    obj->data = *cmd.newData;
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
        
        for (size_t i = 0; i < m_objects.size(); ++i) {
            if (merged.contains(i)) continue;
            
            const auto& obj = m_objects[i].data;
            RectF mergedRect = obj.rect;
            merged.insert(i);
            
            bool foundMerge = true;
            while (foundMerge) {
                foundMerge = false;
                
                for (size_t j = 0; j < m_objects.size(); ++j) {
                    if (merged.contains(j)) continue;
                    
                    const auto& other = m_objects[j].data;
                    
                    if (obj.type != other.type ||
                        obj.groundSide != other.groundSide ||
                        obj.movementType != other.movementType ||
                        Math::Abs(obj.movementSpeed - other.movementSpeed) > epsilon ||
                        Math::Abs(obj.movementDistance - other.movementDistance) > epsilon ||
                        Math::Abs(obj.damageAmount - other.damageAmount) > epsilon) {
                        continue;
                    }
                    
                    const RectF otherRect = other.rect;
                    
                    if (Math::Abs(mergedRect.y - otherRect.y) < epsilon && 
                        Math::Abs(mergedRect.h - otherRect.h) < epsilon) {
                        if (Math::Abs((mergedRect.x + mergedRect.w) - otherRect.x) < epsilon) {
                            mergedRect.w = otherRect.x + otherRect.w - mergedRect.x;
                            merged.insert(j);
                            foundMerge = true;
                        }
                        else if (Math::Abs((otherRect.x + otherRect.w) - mergedRect.x) < epsilon) {
                            double rightEdge = mergedRect.x + mergedRect.w;
                            mergedRect.x = otherRect.x;
                            mergedRect.w = rightEdge - otherRect.x;
                            merged.insert(j);
                            foundMerge = true;
                        }
                    }
                    else if (Math::Abs(mergedRect.x - otherRect.x) < epsilon && 
                             Math::Abs(mergedRect.w - otherRect.w) < epsilon) {
                        if (Math::Abs((mergedRect.y + mergedRect.h) - otherRect.y) < epsilon) {
                            mergedRect.h = otherRect.y + otherRect.h - mergedRect.y;
                            merged.insert(j);
                            foundMerge = true;
                        }
                        else if (Math::Abs((otherRect.y + otherRect.h) - mergedRect.y) < epsilon) {
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
        
        // テクスチャ情報を保存
        if (!obj.texturePath.isEmpty()) {
            json[U"texturePath"] = obj.texturePath;
        }
        
        if (obj.type == Stage::StageType::MovingPlatform ||
            obj.type == Stage::StageType::MovingDamagePlatform) {
            json[U"movementType"] = Stage::movementTypeToString(obj.movementType);
            json[U"movementSpeed"] = static_cast<int>(obj.movementSpeed);
            json[U"movementDistance"] = static_cast<int>(obj.movementDistance);
            json[U"loopMovement"] = obj.loopMovement;
        }
        
        if (obj.type == Stage::StageType::DamagePlatform ||
            obj.type == Stage::StageType::MovingDamagePlatform) {
            json[U"damageAmount"] = static_cast<int>(obj.damageAmount);
        }
        
        return json;
    }
    
    Optional<Stage::StageObject> StageEditorManager::jsonToStageObject(const JSON& json) const
    {
        Stage::StageObject obj;
        
        if (!json.hasElement(U"rect") || !json[U"rect"].isArray()) {
            return none;
        }
        
        const auto& rectArray = json[U"rect"];
        if (rectArray.size() < 4) return none;
        
        obj.rect = RectF{
            rectArray[0].get<double>(),
            rectArray[1].get<double>(),
            rectArray[2].get<double>(),
            rectArray[3].get<double>()
        };
        
        if (json.hasElement(U"type")) {
            obj.type = Stage::stringToCollisionType(json[U"type"].getString());
        }
        if (json.hasElement(U"groundSide")) {
            obj.groundSide = Stage::stringToGroundSide(json[U"groundSide"].getString());
        }
        if (json.hasElement(U"metadata")) {
            obj.metadata = json[U"metadata"].getString();
        }
        
        // テクスチャ情報を読み込み
        if (json.hasElement(U"texturePath")) {
            obj.texturePath = json[U"texturePath"].getString();
        }
        else {
            // texturePathがない場合はデフォルト値を設定
            obj.texturePath = U"Assets/Stage/normal_stage.png";
        }
        
        if (json.hasElement(U"movementType")) {
            obj.movementType = Stage::stringToMovementType(json[U"movementType"].getString());
        }
        if (json.hasElement(U"movementSpeed")) {
            obj.movementSpeed = json[U"movementSpeed"].get<double>();
        }
        if (json.hasElement(U"movementDistance")) {
            obj.movementDistance = json[U"movementDistance"].get<double>();
        }
        if (json.hasElement(U"loopMovement")) {
            obj.loopMovement = json[U"loopMovement"].get<bool>();
        }
        
        if (json.hasElement(U"damageAmount")) {
            obj.damageAmount = json[U"damageAmount"].get<double>();
        }
        
        return obj;
    }
}
