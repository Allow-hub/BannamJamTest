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
        
        // Modify用（変更前後のデータを保存）
        Optional<typename TObject::DataType> oldData;
        Optional<typename TObject::DataType> newData;
    };
    
    // エディタオブジェクトの基底
    template<typename TData>
    struct EditorObjectBase
    {
        using DataType = TData;
        
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
        HashSet<size_t> m_selectedIndices;
        bool m_isExecutingCommand = false;
        
        static constexpr size_t MAX_HISTORY_SIZE = 100;
        
    public:
        virtual ~EditorManagerBase() = default;
        
        // ===== 共通インターフェース =====
        
        // アクセサ
        const Array<TEditorObject>& getAllObjects() const { return m_objects; }
        const HashSet<size_t>& getSelectedIndices() const { return m_selectedIndices; }
        
        Array<const TEditorObject*> getSelectedObjects() const
        {
            Array<const TEditorObject*> result;
            result.reserve(m_selectedIndices.size());
            
            for (size_t idx : m_selectedIndices) {
                if (idx < m_objects.size()) {
                    result.push_back(&m_objects[idx]);
                }
            }
            return result;
        }
        
        // 選択操作
        void selectObject(size_t index, bool additive = false)
        {
            if (!additive) {
                m_selectedIndices.clear();
            }
            
            if (index < m_objects.size()) {
                m_selectedIndices.insert(index);
                updateSelectionStates();
            }
        }
        
        void deselectObject(size_t index)
        {
            m_selectedIndices.erase(index);
            
            if (index < m_objects.size()) {
                m_objects[index].isSelected = false;
            }
        }
        
        void clearSelection()
        {
            m_selectedIndices.clear();
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
            m_selectedIndices.clear();
        }
        
        // ===== 派生クラスで実装する純粋仮想関数 =====
        
        // オブジェクト操作
        virtual size_t addObject(const typename TEditorObject::DataType& data) = 0;
        virtual void removeObject(size_t index) = 0;
        
        // ファイルI/O
        virtual void saveToJSON(const FilePath& path) const = 0;
        virtual void loadFromJSON(const FilePath& path) = 0;
        
    protected:
        // ===== 派生クラス用のユーティリティ =====
        
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
        
        void removeObjectDirect(size_t index)
        {
            if (index < m_objects.size()) {
                m_objects.erase(m_objects.begin() + index);
                
                // 削除後、インデックスを更新
                HashSet<size_t> newSelectedIndices;
                for (size_t selectedIdx : m_selectedIndices) {
                    if (selectedIdx < index) {
                        newSelectedIndices.insert(selectedIdx);
                    } else if (selectedIdx > index) {
                        newSelectedIndices.insert(selectedIdx - 1);
                    }
                }
                m_selectedIndices = newSelectedIndices;
            }
        }
        
        TEditorObject* findObjectByIndex(size_t index)
        {
            if (index < m_objects.size()) {
                return &m_objects[index];
            }
            return nullptr;
        }
        
        const TEditorObject* findObjectByIndex(size_t index) const
        {
            if (index < m_objects.size()) {
                return &m_objects[index];
            }
            return nullptr;
        }
        
        void updateSelectionStates()
        {
            for (size_t i = 0; i < m_objects.size(); ++i) {
                m_objects[i].isSelected = m_selectedIndices.contains(i);
            }
        }
        
        // ===== 派生クラスで実装すべきコマンド実行 =====
        virtual void executeUndoCommand(const TCommand& cmd) = 0;
        virtual void executeRedoCommand(const TCommand& cmd) = 0;
    };
}
