#include "StageEditorManager.h"

namespace Jam::Domain::Editor
{
    size_t StageEditorManager::addObject(const Stage::StageObject& obj)
    {
        StageEditorObject editorObj;
        editorObj.stageObject = obj;
        editorObj.id = m_nextId++;
        
        m_objects.push_back(editorObj);
        
        StageEditorCommand cmd;
        cmd.type = StageEditorCommand::Type::Add;
        cmd.object = editorObj;
        addToHistory(cmd);
        
        return *editorObj.id;
    }

    void StageEditorManager::removeObject(size_t id)
    {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
        {
            if (it->id == id)
            {
                StageEditorCommand cmd;
                cmd.type = StageEditorCommand::Type::Delete;
                cmd.object = *it;
                addToHistory(cmd);
                
                m_objects.erase(it);
                if (m_selectedId == id)
                {
                    m_selectedId.reset();
                }
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
                StageEditorCommand cmd;
                cmd.type = StageEditorCommand::Type::Move;
                cmd.object = obj;
                cmd.oldPos = obj.stageObject.rect.pos;
                cmd.newPos = newPos;
                addToHistory(cmd);
                
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
                StageEditorCommand cmd;
                cmd.type = StageEditorCommand::Type::Modify;
                cmd.object = obj;
                addToHistory(cmd);
                
                obj.stageObject = newObj;
                break;
            }
        }
    }

    void StageEditorManager::selectObject(size_t id)
    {
        for (auto& obj : m_objects)
        {
            obj.isSelected = (obj.id == id);
        }
        m_selectedId = id;
    }

    void StageEditorManager::clearSelection()
    {
        for (auto& obj : m_objects)
        {
            obj.isSelected = false;
        }
        m_selectedId.reset();
    }

    const StageEditorObject* StageEditorManager::getSelectedObject() const
    {
        if (!m_selectedId) return nullptr;
        
        for (const auto& obj : m_objects)
        {
            if (obj.id == *m_selectedId)
            {
                return &obj;
            }
        }
        return nullptr;
    }

    Optional<size_t> StageEditorManager::findObjectAt(const Vec2& pos) const
    {
        for (const auto& obj : m_objects)
        {
            if (obj.stageObject.rect.contains(pos))
            {
                return obj.id;
            }
        }
        return none;
    }

    void StageEditorManager::undo()
    {
        if (!canUndo()) return;
        
        m_historyIndex--;
        const auto& cmd = m_commandHistory[m_historyIndex];
        
        switch (cmd.type)
        {
        case StageEditorCommand::Type::Add:
            if (cmd.object.id)
            {
                removeObject(*cmd.object.id);
                m_historyIndex--;
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
        }
    }

    void StageEditorManager::redo()
    {
        if (!canRedo()) return;
        
        const auto& cmd = m_commandHistory[m_historyIndex];
        executeCommand(cmd);
        m_historyIndex++;
    }

    void StageEditorManager::saveToJSON(const FilePath& path) const
    {
        JSON json;
        json[U"objects"] = Array<JSON>();
        
        for (const auto& editorObj : m_objects)
        {
            const auto& obj = editorObj.stageObject;
            JSON objJson;
            
            // rect配列形式で保存
            objJson[U"rect"] = Array<double>{obj.rect.x, obj.rect.y, obj.rect.w, obj.rect.h};
            
            // type文字列で保存
            String typeStr;
            switch (obj.type) {
                case Stage::StageType::Normal: typeStr = U"solid"; break;
                case Stage::StageType::MovingPlatform: typeStr = U"move"; break;
                case Stage::StageType::OneWayPlatform: typeStr = U"oneway"; break;
                case Stage::StageType::DamagePlatform: typeStr = U"damage"; break;
                case Stage::StageType::MovingDamagePlatform: typeStr = U"movingDamage"; break;
                default: typeStr = U"solid"; break;
            }
            objJson[U"type"] = typeStr;
            
            // groundSide文字列で保存
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
            objJson[U"groundSide"] = groundSideStr;
            
            // metadata
            objJson[U"metadata"] = obj.metadata;
            
            // 動く床の場合、追加パラメータを保存
            if (obj.type == Stage::StageType::MovingPlatform || obj.type == Stage::StageType::MovingDamagePlatform)
            {
                String movementTypeStr;
                switch (obj.movementType) {
                    case Stage::MovementType::Horizontal: movementTypeStr = U"horizontal"; break;
                    case Stage::MovementType::Vertical: movementTypeStr = U"vertical"; break;
                    case Stage::MovementType::Circular: movementTypeStr = U"circular"; break;
                    default: movementTypeStr = U"horizontal"; break;
                }
                objJson[U"movementType"] = movementTypeStr;
                objJson[U"movementSpeed"] = obj.movementSpeed;
                objJson[U"movementDistance"] = obj.movementDistance;
                objJson[U"loopMovement"] = obj.loopMovement;
            }
            
            // ダメージ床の場合、ダメージ量を保存
            if (obj.type == Stage::StageType::DamagePlatform || obj.type == Stage::StageType::MovingDamagePlatform)
            {
                objJson[U"damageAmount"] = obj.damageAmount;
            }
            
            json[U"objects"].push_back(objJson);
        }
        
        json.save(path);
    }

    void StageEditorManager::loadFromJSON(const FilePath& path)
    {
        clear();
        
        JSON json = JSON::Load(path);
        if (!json) return;
        
        // "objects"キーから読み込み(既存のステージJSONと同じ形式)
        if (!json.hasElement(U"objects")) return;
        
        const auto& objects = json[U"objects"];
        for (const auto& objJson : objects.arrayView())
        {
            Stage::StageObject obj;
            
            // rect配列から読み込み
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
            
            // type文字列から変換
            if (objJson.hasElement(U"type"))
            {
                obj.type = Stage::stringToCollisionType(objJson[U"type"].getString());
            }
            
            // groundSide文字列から変換
            if (objJson.hasElement(U"groundSide"))
            {
                obj.groundSide = Stage::stringToGroundSide(objJson[U"groundSide"].getString());
            }
            
            // metadata
            if (objJson.hasElement(U"metadata"))
            {
                obj.metadata = objJson[U"metadata"].getString();
            }
            
            // 動く床のパラメータ
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
            
            // ダメージ量
            if (objJson.hasElement(U"damageAmount"))
            {
                obj.damageAmount = objJson[U"damageAmount"].get<double>();
            }
            
            addObject(obj);
        }
    }

    void StageEditorManager::clear()
    {
        m_objects.clear();
        m_commandHistory.clear();
        m_historyIndex = 0;
        m_selectedId.reset();
    }

    void StageEditorManager::executeCommand(const StageEditorCommand& cmd)
    {
        switch (cmd.type)
        {
        case StageEditorCommand::Type::Add:
            m_objects.push_back(cmd.object);
            break;
            
        case StageEditorCommand::Type::Delete:
            if (cmd.object.id)
            {
                removeObject(*cmd.object.id);
            }
            break;
            
        case StageEditorCommand::Type::Move:
            if (cmd.object.id)
            {
                moveObject(*cmd.object.id, cmd.newPos);
            }
            break;
        }
    }

    void StageEditorManager::addToHistory(const StageEditorCommand& cmd)
    {
        m_commandHistory.resize(m_historyIndex);
        m_commandHistory.push_back(cmd);
        m_historyIndex++;
    }
}