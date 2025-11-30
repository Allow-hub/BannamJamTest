#pragma once
#include <Siv3D.hpp>
#include "Domain/Editor/EditorCameraController.h"

namespace Jam::UseCase::Editor
{
    // エディタモードの前方宣言を使用
    using Jam::Domain::Editor::EditorCameraController;
    
    // エディタモード
    enum class EditorMode
    {
        Select,
        Place,
        Delete
    };
    
    // Service基底クラス
    template<typename TManager>
    class EditorServiceBase
    {
    protected:
        TManager m_manager;
        EditorCameraController* m_cameraController = nullptr;
        EditorCameraController m_ownCameraController;  // デフォルトのカメラ
        EditorMode m_mode = EditorMode::Place;
        
    public:
        virtual ~EditorServiceBase() = default;
        
        // カメラコントローラを外部から設定（共有用）
        void setSharedCameraController(EditorCameraController* controller) 
        { 
            m_cameraController = controller; 
        }
        
        // 実際に使用するカメラコントローラを取得
        EditorCameraController& getCameraController()
        {
            return m_cameraController ? *m_cameraController : m_ownCameraController;
        }
        
        const EditorCameraController& getCameraController() const
        {
            return m_cameraController ? *m_cameraController : m_ownCameraController;
        }
        
        // ===== 共通インターフェース =====
        
        // モード操作
        void setMode(EditorMode mode) { m_mode = mode; }
        EditorMode getMode() const { return m_mode; }
        
        // カメラ操作
        void updateCamera() { getCameraController().update(); }
        const Camera2D& getCamera() const { return getCameraController().getCamera(); }
        Vec2 screenToWorld(const Vec2& screenPos) const { 
            return getCameraController().screenToWorld(screenPos); 
        }
        auto createCameraTransformer() const { 
            return getCameraController().createTransformer(); 
        }
        
        void setCameraSpeed(double speed) { getCameraController().setSpeed(speed); }
        double getCameraSpeed() const { return getCameraController().getSpeed(); }
        
        // マネージャーアクセス
        const TManager& getManager() const { return m_manager; }
        TManager& getManager() { return m_manager; }
        
        // Undo/Redo
        void undo() { m_manager.undo(); }
        void redo() { m_manager.redo(); }
        bool canUndo() const { return m_manager.canUndo(); }
        bool canRedo() const { return m_manager.canRedo(); }
        
        // ファイル操作
        void save(const FilePath& path) { m_manager.saveToJSON(path); }
        void load(const FilePath& path) { m_manager.loadFromJSON(path); }
        void clear() { m_manager.clear(); }
        
        // ===== 派生クラスで実装する操作 =====
        
        virtual void handlePlacement(const Vec2& mousePos) = 0;
        virtual void handleSelection(const Vec2& mousePos) = 0;
        virtual void handleDeletion(const Vec2& mousePos) = 0;
        
        // メイン更新処理（テンプレートメソッドパターン）
        void handleInput()
        {
            if (MouseL.down())
            {
                const Vec2 mousePos = screenToWorld(Cursor::Pos());
                
                switch (m_mode)
                {
                case EditorMode::Place:
                    handlePlacement(mousePos);
                    break;
                case EditorMode::Select:
                    handleSelection(mousePos);
                    break;
                case EditorMode::Delete:
                    handleDeletion(mousePos);
                    break;
                }
            }
        }
    };
}
