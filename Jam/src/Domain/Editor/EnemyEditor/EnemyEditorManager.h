#pragma once
#include "EnemyEditorTypes.h"

namespace Jam::Domain::Editor
{
    class EnemyEditorManager
    {
    private:
        Array<EnemyEditorObject> m_enemies;
        Array<EnemyEditorCommand> m_commandHistory;
        size_t m_historyIndex = 0;
        size_t m_nextId = 0;
        HashSet<size_t> m_selectedIds;
        
        bool m_isExecutingCommand = false;
        
    public:
        size_t addEnemy(const EnemyObject& enemy);
        void removeEnemy(size_t id);
        void modifyEnemy(size_t id, const EnemyObject& newEnemy);
        
        void selectEnemy(size_t id, bool additive = false);
        void deselectEnemy(size_t id);
        void clearSelection();
        
        const HashSet<size_t>& getSelectedIds() const { return m_selectedIds; }
        Array<const EnemyEditorObject*> getSelectedEnemies() const;
        const Array<EnemyEditorObject>& getAllEnemies() const { return m_enemies; }
        Optional<size_t> findEnemyAt(const Vec2& pos) const;
        
        void undo();
        void redo();
        bool canUndo() const { return m_historyIndex > 0; }
        bool canRedo() const { return m_historyIndex < m_commandHistory.size(); }
        
        void saveToJSON(const FilePath& path) const;
        void loadFromJSON(const FilePath& path);
        
        void clear();
        
    private:
        void executeCommand(const EnemyEditorCommand& cmd);
        void addToHistory(const EnemyEditorCommand& cmd);
        void removeEnemyDirect(size_t id);
    };
}
