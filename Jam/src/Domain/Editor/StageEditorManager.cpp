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
        json[U"stage"][U"objects"] = Array<JSON>();
        
        for (const auto& editorObj : m_objects)
        {
            const auto& obj = editorObj.stageObject;
            JSON objJson;
            objJson[U"x"] = obj.rect.x;
            objJson[U"y"] = obj.rect.y;
            objJson[U"width"] = obj.rect.w;
            objJson[U"height"] = obj.rect.h;
            objJson[U"groundSide"] = static_cast<int>(obj.groundSide);
            objJson[U"type"] = static_cast<int>(obj.type);
            objJson[U"metadata"] = obj.metadata;
            
            json[U"stage"][U"objects"].push_back(objJson);
        }
        
        json.save(path);
    }

    void StageEditorManager::loadFromJSON(const FilePath& path)
    {
        clear();
        
        JSON json = JSON::Load(path);
        if (!json) return;
        
        const auto& objects = json[U"stage"][U"objects"];
        for (const auto& objJson : objects.arrayView())
        {
            Stage::StageObject obj;
            obj.rect = RectF{
                objJson[U"x"].get<double>(),
                objJson[U"y"].get<double>(),
                objJson[U"width"].get<double>(),
                objJson[U"height"].get<double>()
            };
            obj.groundSide = static_cast<Stage::GroundSide>(objJson[U"groundSide"].get<int>());
            obj.type = static_cast<Stage::StageType>(objJson[U"type"].get<int>());
            obj.metadata = objJson[U"metadata"].get<String>();
            
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