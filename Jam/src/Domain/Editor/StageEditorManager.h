#pragma once
#include "StageEditorTypes.h"

namespace Jam::Domain::Editor
{
    class StageEditorManager
    {
    private:
        Array<StageEditorObject> m_objects;
        Array<StageEditorCommand> m_commandHistory;
        size_t m_historyIndex = 0;
        size_t m_nextId = 0;
        HashSet<size_t> m_selectedIds;
        
        bool m_isExecutingCommand = false;
        
    public:
        size_t addObject(const Stage::StageObject& obj);
        void removeObject(size_t id);
        void moveObject(size_t id, const Vec2& newPos);
        void modifyObject(size_t id, const Stage::StageObject& newObj);
        
        void selectObject(size_t id, bool additive = false);
        void deselectObject(size_t id);
        void clearSelection();
        
        const HashSet<size_t>& getSelectedIds() const { return m_selectedIds; }
        Array<const StageEditorObject*> getSelectedObjects() const;
        const Array<StageEditorObject>& getAllObjects() const { return m_objects; }
        Optional<size_t> findObjectAt(const Vec2& pos) const;
        
		bool hasObjectAtExactPosition(const RectF& rect) const;

        void undo();
        void redo();
        bool canUndo() const { return m_historyIndex > 0; }
        bool canRedo() const { return m_historyIndex < m_commandHistory.size(); }
        
        void saveToJSON(const FilePath& path) const;
        void loadFromJSON(const FilePath& path);
        
        void clear();
        
    private:
        void executeCommand(const StageEditorCommand& cmd);
        void addToHistory(const StageEditorCommand& cmd);
        
        void removeObjectDirect(size_t id);
        
        Array<Stage::StageObject> mergeAdjacentObjects() const;
    };
}