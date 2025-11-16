#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Editor
{
    // コマンドの基底
    template<typename TObject>
    struct EditorCommandBase
    {
        enum class Type { Add, Delete, Move, Modify };
        Type type;
        TObject object;
        
        // Move用（オプション）
        Optional<Vec2> oldPos;
        Optional<Vec2> newPos;
        
        // Modify用（改善：新旧データを保存）
        Optional<typename TObject::DataType> oldData;
        Optional<typename TObject::DataType> newData;
    };
    
    // エディタオブジェクトの基底
    template<typename TData>
    struct EditorObjectBase
    {
        using DataType = TData;
        
        Optional<size_t> id;
        TData data;
        bool isSelected = false;
        
        EditorObjectBase() = default;
        explicit EditorObjectBase(const TData& d) : data(d) {}
    };
    
    // Manager基底クラス
    template<typename TEditorObject, typename TCommand>
    class EditorManagerBase
    {
    protected:
        Array<TEditorObject> m_objects;
        Array<TCommand> m_commandHistory;
        size_t m_historyIndex = 0;
        size_t m_nextId = 0;
        HashSet<size_t> m_selectedIds;
        bool m_isExecutingCommand = false;
        
        static constexpr size_t MAX_HISTORY_SIZE = 100;
        
    public:
        virtual ~EditorManagerBase() = default;
        
        // ===== 共通インターフェース =====
        
        // アクセサ
        const Array<TEditorObject>& getAllObjects() const { return m_objects; }
        const HashSet<size_t>& getSelectedIds() const { return m_selectedIds; }
        
        Array<const TEditorObject*> getSelectedObjects() const
        {
            Array<const TEditorObject*> result;
            result.reserve(m_selectedIds.size());
            
            for (const auto& obj : m_objects) {
                if (obj.id && m_selectedIds.contains(*obj.id)) {
                    result.push_back(&obj);
                }
            }
            return result;
        }
        
        // 選択操作
        void selectObject(size_t id, bool additive = false)
        {
            if (!additive) {
                m_selectedIds.clear();
            }
            
            m_selectedIds.insert(id);
            updateSelectionStates();
        }
        
        void deselectObject(size_t id)
        {
            m_selectedIds.erase(id);
            
            for (auto& obj : m_objects) {
                if (obj.id == id) {
                    obj.isSelected = false;
                    break;
                }
            }
        }
        
        void clearSelection()
        {
            m_selectedIds.clear();
            for (auto& obj : m_objects) {
                obj.isSelected = false;
            }
        }
        
        // Undo/Redo
        bool canUndo() const { return m_historyIndex > 0; }
        bool canRedo() const { return m_historyIndex < m_commandHistory.size(); }
        
        void undo()
        {
            if (!canUndo()) return;
            
            m_isExecutingCommand = true;
            m_historyIndex--;
            
            executeUndoCommand(m_commandHistory[m_historyIndex]);
            
            m_isExecutingCommand = false;
        }
        
        void redo()
        {
            if (!canRedo()) return;
            
            m_isExecutingCommand = true;
            const auto& cmd = m_commandHistory[m_historyIndex];
            m_historyIndex++;
            
            executeRedoCommand(cmd);
            
            m_isExecutingCommand = false;
        }
        
        // クリア
        void clear()
        {
            m_objects.clear();
            m_commandHistory.clear();
            m_historyIndex = 0;
            m_nextId = 0;
            m_selectedIds.clear();
        }
        
        // ===== 派生クラスで実装する純粋仮想関数 =====
        
        // オブジェクト操作
        virtual size_t addObject(const typename TEditorObject::DataType& data) = 0;
        virtual void removeObject(size_t id) = 0;
        
        // ファイルI/O
        virtual void saveToJSON(const FilePath& path) const = 0;
        virtual void loadFromJSON(const FilePath& path) = 0;
        
    protected:
        // ===== 派生クラス用のユーティリティ =====
        
        size_t getNextId() { return m_nextId++; }
        
        void addToHistory(const TCommand& cmd)
        {
            if (m_isExecutingCommand) return;
            
            // 現在位置以降の履歴を削除
            m_commandHistory.resize(m_historyIndex);
            m_commandHistory.push_back(cmd);
            m_historyIndex++;
            
            // 履歴サイズ制限
            if (m_commandHistory.size() > MAX_HISTORY_SIZE) {
                m_commandHistory.erase(m_commandHistory.begin());
                m_historyIndex--;
            }
        }
        
        void removeObjectDirect(size_t id)
        {
            for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
                if (it->id == id) {
                    m_objects.erase(it);
                    m_selectedIds.erase(id);
                    break;
                }
            }
        }
        
        TEditorObject* findObjectById(size_t id)
        {
            for (auto& obj : m_objects) {
                if (obj.id == id) return &obj;
            }
            return nullptr;
        }
        
        const TEditorObject* findObjectById(size_t id) const
        {
            for (const auto& obj : m_objects) {
                if (obj.id == id) return &obj;
            }
            return nullptr;
        }
        
        void updateSelectionStates()
        {
            for (auto& obj : m_objects) {
                obj.isSelected = obj.id && m_selectedIds.contains(*obj.id);
            }
        }
        
        // ===== 派生クラスで実装すべきコマンド実行 =====
        virtual void executeUndoCommand(const TCommand& cmd) = 0;
        virtual void executeRedoCommand(const TCommand& cmd) = 0;
    };
}
