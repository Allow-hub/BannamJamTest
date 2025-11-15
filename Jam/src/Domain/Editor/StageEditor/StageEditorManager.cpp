#include "StageEditorManager.h"

namespace Jam::Domain::Editor
{
    size_t StageEditorManager::addObject(const Stage::StageObject& obj)
    {
        StageEditorObject editorObj;
        editorObj.stageObject = obj;
        editorObj.id = m_nextId++;
        
        m_objects.push_back(editorObj);
        
        if (!m_isExecutingCommand)
        {
            StageEditorCommand cmd;
            cmd.type = StageEditorCommand::Type::Add;
            cmd.object = editorObj;
            addToHistory(cmd);
        }
        
        return *editorObj.id;
    }

    void StageEditorManager::removeObject(size_t id)
    {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
        {
            if (it->id == id)
            {
                if (!m_isExecutingCommand)
                {
                    StageEditorCommand cmd;
                    cmd.type = StageEditorCommand::Type::Delete;
                    cmd.object = *it;
                    addToHistory(cmd);
                }
                
                m_objects.erase(it);
                m_selectedIds.erase(id);
                break;
            }
        }
    }

    void StageEditorManager::removeObjectDirect(size_t id)
    {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
        {
            if (it->id == id)
            {
                m_objects.erase(it);
                m_selectedIds.erase(id);
                break;
            }
        }
    }

    void StageEditorManager::moveObject(size_t id, const Vec2& newPos)
    {
        for (auto& obj : m_objects)
        {
            if (obj.id == id)
            {
                if (!m_isExecutingCommand)
                {
                    StageEditorCommand cmd;
                    cmd.type = StageEditorCommand::Type::Move;
                    cmd.object = obj;
                    cmd.oldPos = obj.stageObject.rect.pos;
                    cmd.newPos = newPos;
                    addToHistory(cmd);
                }
                
                obj.stageObject.rect.setPos(newPos);
                break;
            }
        }
    }

    void StageEditorManager::modifyObject(size_t id, const Stage::StageObject& newObj)
    {
        for (auto& obj : m_objects)
        {
            if (obj.id == id)
            {
                if (!m_isExecutingCommand)
                {
                    StageEditorCommand cmd;
                    cmd.type = StageEditorCommand::Type::Modify;
                    cmd.object = obj;
                    addToHistory(cmd);
                }
                
                obj.stageObject = newObj;
                break;
            }
        }
    }

    void StageEditorManager::selectObject(size_t id, bool additive)
    {
        if (!additive)
        {
            m_selectedIds.clear();
        }
        
        m_selectedIds.insert(id);
        
        for (auto& obj : m_objects)
        {
            obj.isSelected = m_selectedIds.contains(*obj.id);
        }
    }
    
    void StageEditorManager::deselectObject(size_t id)
    {
        m_selectedIds.erase(id);
        
        for (auto& obj : m_objects)
        {
            if (obj.id == id)
            {
                obj.isSelected = false;
                break;
            }
        }
    }

    void StageEditorManager::clearSelection()
    {
        m_selectedIds.clear();
        
        for (auto& obj : m_objects)
        {
            obj.isSelected = false;
        }
    }

    Array<const StageEditorObject*> StageEditorManager::getSelectedObjects() const
    {
        Array<const StageEditorObject*> result;
        
        for (const auto& obj : m_objects)
        {
            if (m_selectedIds.contains(*obj.id))
            {
                result.push_back(&obj);
            }
        }
        
        return result;
    }

    void StageEditorManager::updateSelectedObjectsMovement(double distance, double speed, Domain::Stage::MovementType type)
    {
        for (auto& obj : m_objects)
        {
            if (m_selectedIds.contains(*obj.id))
            {
                if (obj.stageObject.type == Stage::StageType::MovingPlatform ||
                    obj.stageObject.type == Stage::StageType::MovingDamagePlatform)
                {
                    obj.stageObject.movementDistance = distance;
                    obj.stageObject.movementSpeed = speed;
                    obj.stageObject.movementType = type;
                }
            }
        }
    }

    void StageEditorManager::updateSelectedObjectsDamage(double damage)
    {
        for (auto& obj : m_objects)
        {
            if (m_selectedIds.contains(*obj.id))
            {
                if (obj.stageObject.type == Stage::StageType::DamagePlatform ||
                    obj.stageObject.type == Stage::StageType::MovingDamagePlatform)
                {
                    obj.stageObject.damageAmount = damage;
                }
            }
        }
    }

    Optional<size_t> StageEditorManager::findObjectAt(const Vec2& pos) const
    {
        for (auto it = m_objects.rbegin(); it != m_objects.rend(); ++it)
        {
            if (it->stageObject.rect.contains(pos))
            {
                return it->id;
            }
        }
        return none;
    }
    
    bool StageEditorManager::hasObjectAtExactPosition(const RectF& rect) const
    {
        for (const auto& obj : m_objects)
        {
            if (obj.stageObject.rect.x == rect.x &&
                obj.stageObject.rect.y == rect.y &&
                obj.stageObject.rect.w == rect.w &&
                obj.stageObject.rect.h == rect.h)
            {
                return true;
            }
        }
        return false;
    }

    void StageEditorManager::undo()
    {
        if (!canUndo()) return;
        
        m_isExecutingCommand = true;
        m_historyIndex--;
        const auto& cmd = m_commandHistory[m_historyIndex];
        
        switch (cmd.type)
        {
        case StageEditorCommand::Type::Add:
            if (cmd.object.id)
            {
                removeObjectDirect(*cmd.object.id);
            }
            break;
            
        case StageEditorCommand::Type::Delete:
            m_objects.push_back(cmd.object);
            break;
            
        case StageEditorCommand::Type::Move:
            if (cmd.object.id)
            {
                for (auto& obj : m_objects)
                {
                    if (obj.id == *cmd.object.id)
                    {
                        obj.stageObject.rect.setPos(cmd.oldPos);
                        break;
                    }
                }
            }
            break;
            
        case StageEditorCommand::Type::Modify:
            if (cmd.object.id)
            {
                for (auto& obj : m_objects)
                {
                    if (obj.id == *cmd.object.id)
                    {
                        obj.stageObject = cmd.object.stageObject;
                        break;
                    }
                }
            }
            break;
        }
        
        m_isExecutingCommand = false;
    }

    void StageEditorManager::redo()
    {
        if (!canRedo()) return;
        
        m_isExecutingCommand = true;
        const auto& cmd = m_commandHistory[m_historyIndex];
        m_historyIndex++;
        
        switch (cmd.type)
        {
        case StageEditorCommand::Type::Add:
            m_objects.push_back(cmd.object);
            break;
            
        case StageEditorCommand::Type::Delete:
            if (cmd.object.id)
            {
                removeObjectDirect(*cmd.object.id);
            }
            break;
            
        case StageEditorCommand::Type::Move:
            if (cmd.object.id)
            {
                for (auto& obj : m_objects)
                {
                    if (obj.id == *cmd.object.id)
                    {
                        obj.stageObject.rect.setPos(cmd.newPos);
                        break;
                    }
                }
            }
            break;
            
        case StageEditorCommand::Type::Modify:
            break;
        }
        
        m_isExecutingCommand = false;
    }

    void StageEditorManager::saveToJSON(const FilePath& path) const
    {
        Array<Stage::StageObject> mergedObjects = mergeAdjacentObjects();
        
        String output = U"{\n  \"objects\": [\n";
        
        for (size_t i = 0; i < mergedObjects.size(); ++i)
        {
            const auto& obj = mergedObjects[i];
            
            String typeStr;
            switch (obj.type) {
                case Stage::StageType::Normal: typeStr = U"solid"; break;
                case Stage::StageType::MovingPlatform: typeStr = U"move"; break;
                case Stage::StageType::OneWayPlatform: typeStr = U"oneway"; break;
                case Stage::StageType::DamagePlatform: typeStr = U"damage"; break;
                case Stage::StageType::MovingDamagePlatform: typeStr = U"movingDamage"; break;
                default: typeStr = U"solid"; break;
            }
            
            String groundSideStr;
            switch (obj.groundSide) {
                case Stage::GroundSide::None: groundSideStr = U"none"; break;
                case Stage::GroundSide::Up: groundSideStr = U"up"; break;
                case Stage::GroundSide::Down: groundSideStr = U"down"; break;
                case Stage::GroundSide::Left: groundSideStr = U"left"; break;
                case Stage::GroundSide::Right: groundSideStr = U"right"; break;
                case Stage::GroundSide::All: groundSideStr = U"All"; break;
                default: groundSideStr = U"All"; break;
            }
            
            output += U"    {\n";
            output += U"      \"rect\": [ ";
            output += U"{}"_fmt(static_cast<int>(obj.rect.x)) + U", ";
            output += U"{}"_fmt(static_cast<int>(obj.rect.y)) + U", ";
            output += U"{}"_fmt(static_cast<int>(obj.rect.w)) + U", ";
            output += U"{}"_fmt(static_cast<int>(obj.rect.h)) + U" ],\n";
            output += U"      \"type\": \"" + typeStr + U"\",\n";
            output += U"      \"groundSide\": \"" + groundSideStr + U"\",\n";
            output += U"      \"metadata\": \"" + obj.metadata + U"\"";
            
            if (obj.type == Stage::StageType::MovingPlatform || obj.type == Stage::StageType::MovingDamagePlatform)
            {
                String movementTypeStr;
                switch (obj.movementType) {
                    case Stage::MovementType::Horizontal: movementTypeStr = U"horizontal"; break;
                    case Stage::MovementType::Vertical: movementTypeStr = U"vertical"; break;
                    case Stage::MovementType::Circular: movementTypeStr = U"circular"; break;
                    default: movementTypeStr = U"horizontal"; break;
                }
                output += U",\n      \"movementType\": \"" + movementTypeStr + U"\"";
                output += U",\n      \"movementSpeed\": ";
                output += U"{}"_fmt(static_cast<int>(obj.movementSpeed));
                output += U",\n      \"movementDistance\": ";
                output += U"{}"_fmt(static_cast<int>(obj.movementDistance));
                output += U",\n      \"loopMovement\": " + String(obj.loopMovement ? U"true" : U"false");
            }
            
            if (obj.type == Stage::StageType::DamagePlatform || obj.type == Stage::StageType::MovingDamagePlatform)
            {
                output += U",\n      \"damageAmount\": ";
                output += U"{}"_fmt(static_cast<int>(obj.damageAmount));
            }
            
            output += U"\n    }";
            if (i < mergedObjects.size() - 1)
            {
                output += U",";
            }
            output += U"\n";
        }
        
        output += U"  ]\n}";
        
        TextWriter writer(path);
        if (writer)
        {
            writer.write(output);
        }
    }

    void StageEditorManager::loadFromJSON(const FilePath& path)
    {
        clear();
        
        JSON json = JSON::Load(path);
        if (!json) return;
        
        if (!json.hasElement(U"objects")) return;
        
        const auto& objects = json[U"objects"];
        
        m_isExecutingCommand = true;
        
        for (const auto& objJson : objects.arrayView())
        {
            Stage::StageObject obj;
            
            if (objJson.hasElement(U"rect") && objJson[U"rect"].isArray())
            {
                const auto& rectArray = objJson[U"rect"];
                obj.rect = RectF{
                    rectArray[0].get<double>(),
                    rectArray[1].get<double>(),
                    rectArray[2].get<double>(),
                    rectArray[3].get<double>()
                };
            }
            
            if (objJson.hasElement(U"type"))
            {
                obj.type = Stage::stringToCollisionType(objJson[U"type"].getString());
            }
            
            if (objJson.hasElement(U"groundSide"))
            {
                obj.groundSide = Stage::stringToGroundSide(objJson[U"groundSide"].getString());
            }
            
            if (objJson.hasElement(U"metadata"))
            {
                obj.metadata = objJson[U"metadata"].getString();
            }
            
            if (objJson.hasElement(U"movementType"))
            {
                obj.movementType = Stage::stringToMovementType(objJson[U"movementType"].getString());
            }
            if (objJson.hasElement(U"movementSpeed"))
            {
                obj.movementSpeed = objJson[U"movementSpeed"].get<double>();
            }
            if (objJson.hasElement(U"movementDistance"))
            {
                obj.movementDistance = objJson[U"movementDistance"].get<double>();
            }
            if (objJson.hasElement(U"loopMovement"))
            {
                obj.loopMovement = objJson[U"loopMovement"].get<bool>();
            }
            
            if (objJson.hasElement(U"damageAmount"))
            {
                obj.damageAmount = objJson[U"damageAmount"].get<double>();
            }
            
            addObject(obj);
        }
        
        m_isExecutingCommand = false;
    }

    void StageEditorManager::clear()
    {
        m_objects.clear();
        m_commandHistory.clear();
        m_historyIndex = 0;
        m_selectedIds.clear();
    }

    void StageEditorManager::addToHistory(const StageEditorCommand& cmd)
    {
        constexpr size_t maxHistorySize = 100;
        
        m_commandHistory.resize(m_historyIndex);
        m_commandHistory.push_back(cmd);
        m_historyIndex++;
        
        if (m_commandHistory.size() > maxHistorySize)
        {
            m_commandHistory.erase(m_commandHistory.begin());
            m_historyIndex--;
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
            
            const auto& obj = m_objects[i].stageObject;
            RectF mergedRect = obj.rect;
            merged.insert(i);
            
            bool foundMerge = true;
            while (foundMerge)
            {
                foundMerge = false;
                
                for (size_t j = 0; j < m_objects.size(); ++j)
                {
                    if (merged.contains(j)) continue;
                    
                    const auto& other = m_objects[j].stageObject;
                    
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
}