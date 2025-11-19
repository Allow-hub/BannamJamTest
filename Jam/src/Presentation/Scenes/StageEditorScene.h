#pragma once
#include <Siv3D.hpp>
#include "TitleScene.h"
#include "../Editor/StageEditor/StageEditorRenderer.h"
#include "../Editor/EnemyEditor/EnemyEditorRenderer.h"
#include "../../UseCase/Editor/StageEditor/StageEditorService.h"
#include "../../UseCase/Editor/EnemyEditor/EnemyEditorService.h"
#include "../../Domain/Editor/EditorTypes.h"

namespace Jam::Presentation::Scenes
{
    using App = SceneManager<String>;

    
    class StageEditorScene : public App::Scene
    {
    private:
        Domain::Editor::EditorTarget m_editorTarget = Domain::Editor::EditorTarget::Stage;
        
        // 共有カメラコントローラ
        UseCase::Editor::EditorCameraController m_sharedCamera;
        
        UseCase::Editor::StageEditorService m_stageEditorService;
        Editor::StageEditorRenderer m_stageRenderer;
        
        UseCase::Editor::EnemyEditorService m_enemyEditorService;
        Editor::EnemyEditorRenderer m_enemyRenderer;
        
    public:
        StageEditorScene(const InitData& init)
            : IScene{ init }
        {
            // 両方のエディタに共有カメラを設定
            m_stageEditorService.setSharedCameraController(&m_sharedCamera);
            m_enemyEditorService.setSharedCameraController(&m_sharedCamera);
            
            m_stageRenderer.init(&m_stageEditorService);
            m_enemyRenderer.init(&m_enemyEditorService);
            
            // エディタ起動時にリソースを事前ロード（テストプレイ時の待ち時間短縮）
            preloadGameResources();
            
            // 起動時に両方のJSONファイルを開くダイアログを表示
            loadBothJsonFiles();
        }
        
    private:
        // 両方のJSONファイルを読み込む
        void loadBothJsonFiles()
        {
            // まずステージファイルを選択
            const auto stagePath = Dialog::OpenFile({ FileFilter::JSON() }, U"Assets/Stage/");
            if (!stagePath)
            {
                // キャンセルされた場合は何もしない
                return;
            }
            
            m_stageEditorService.load(*stagePath);
            
            // 次に敵ファイルを選択（同名ファイルが存在する場合はデフォルトで選択）
            FilePath defaultEnemyPath = FileSystem::ParentPath(*stagePath) + U"../Enemy/" + FileSystem::FileName(*stagePath);
            
            const auto enemyPath = Dialog::OpenFile({ FileFilter::JSON() }, U"Assets/Enemy/");
            if (!enemyPath)
            {
                // 敵ファイルがキャンセルされた場合でも、同名ファイルがあれば自動ロード
                if (FileSystem::Exists(defaultEnemyPath))
                {
                    m_enemyEditorService.load(defaultEnemyPath);
                }
                return;
            }
            
            m_enemyEditorService.load(*enemyPath);
        }
        
        // ゲームリソースの事前ロード
        void preloadGameResources()
        {
            // プレイヤーアニメーションを事前ロード
            ResourceManager::initPlayerIdle();
            ResourceManager::initPlayerWalk();
            ResourceManager::initPlayerJump();
            ResourceManager::initPlayerChoker();
            
            ResourceManager::loadGroups({
                ResourceGroup::PlayerIdle,
                ResourceGroup::PlayerWalk,
                ResourceGroup::PlayerJump,
                ResourceGroup::PlayerChoker
            });
            
            // その他のリソースも必要に応じてロード可能
            // ResourceManager::loadGroup(ResourceGroup::Enemy);
            // ResourceManager::loadGroup(ResourceGroup::Stage);
        }
        
        void update() override
        {
            if (KeyEscape.down())
            {
                System::Exit();
            }
            
            // Editor Target切り替え (Tab or GUI Button)
            if (KeyTab.down())
            {
                m_editorTarget = (m_editorTarget == Domain::Editor::EditorTarget::Stage) 
                    ? Domain::Editor::EditorTarget::Enemy 
                    : Domain::Editor::EditorTarget::Stage;
            }
            
            // GUIボタンからの切り替え
            if (m_editorTarget == Domain::Editor::EditorTarget::Stage && m_stageRenderer.isEditorSwitchRequested())
            {
                m_editorTarget = Domain::Editor::EditorTarget::Enemy;
                m_stageRenderer.resetEditorSwitchRequest();
            }
            else if (m_editorTarget == Domain::Editor::EditorTarget::Enemy && m_enemyRenderer.isEditorSwitchRequested())
            {
                m_editorTarget = Domain::Editor::EditorTarget::Stage;
                m_enemyRenderer.resetEditorSwitchRequest();
            }
            
            if (m_editorTarget == Domain::Editor::EditorTarget::Stage)
            {
                updateStageEditor();
            }
            else
            {
                updateEnemyEditor();
            }
        }
        
        void updateStageEditor()
        {
            if (KeyControl.pressed() && KeyP.down())
            {
                m_stageEditorService.save(U"Assets/Stage/stage_edited.json");
                
                auto& core = Jam::Foundation::CoreManager::Instance();
                core.stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;
                
                changeScene(ToSceneString(SceneName::InGame));
                return;
            }
            
            // テキスト入力中でない場合のみカメラ更新
            if (!m_stageRenderer.isTextInputActive())
            {
                m_stageEditorService.updateCamera();
            }
            
            // Alt押下中はモード切り替えしない（Alt+Tabなどのシステムショートカット対策）
            if (!KeyAlt.pressed())
            {
                if (Key1.down()) m_stageEditorService.setMode(UseCase::Editor::EditorMode::Select);
                if (Key2.down()) m_stageEditorService.setMode(UseCase::Editor::EditorMode::Place);
                if (Key3.down()) m_stageEditorService.setMode(UseCase::Editor::EditorMode::Delete);
            }
            
            bool hasMouseInput = MouseL.down() || MouseL.pressed() || MouseL.up();
            
            // パネルが折りたたまれている場合は、折りたたみボタン部分のみをGUI領域とする
            const bool isMouseOverGUI = m_stageRenderer.isMouseOverPanel();
            
            if (hasMouseInput && !isMouseOverGUI)
            {
                Vec2 mousePos = m_stageEditorService.screenToWorld(Cursor::Pos());
                
                switch (m_stageEditorService.getMode())
                {
                case UseCase::Editor::EditorMode::Place:
                    if (MouseL.down() || MouseL.pressed() || MouseL.up())
                    {
                        m_stageEditorService.handlePlacement(mousePos);
                    }
                    break;
                    
                case UseCase::Editor::EditorMode::Select:
                    if (MouseL.down() || MouseL.pressed() || MouseL.up())
                    {
                        m_stageEditorService.handleSelection(mousePos);
                    }
                    break;
                case UseCase::Editor::EditorMode::Delete:
                    if (MouseL.down() || MouseL.pressed())
                    {
                        m_stageEditorService.handleDeletion(mousePos);
                    }
                    break;
                }
            }
            
            if (KeyControl.pressed() && KeyZ.down())
            {
                m_stageEditorService.undo();
            }
            if (KeyControl.pressed() && KeyY.down())
            {
                m_stageEditorService.redo();
            }
            
            // Deleteキーで選択中のオブジェクトを削除
            if (KeyDelete.down())
            {
                auto selectedIds = m_stageEditorService.getManager().getSelectedIds();
                for (auto selectedId : selectedIds)
                {
                    m_stageEditorService.getManager().removeObject(selectedId);
                }
            }
            
            if (KeyControl.pressed() && KeyS.down())
            {
                if (const auto path = Dialog::SaveFile({ FileFilter::JSON() }, U"Assets/Stage/"))
                {
                    m_stageEditorService.save(*path);
                }
            }
            if (KeyControl.pressed() && KeyShift.pressed() && KeyO.down())
            {
                // 両方のJSONファイルを開く
                loadBothJsonFiles();
            }
            else if (KeyControl.pressed() && KeyO.down())
            {
                if (const auto path = Dialog::OpenFile({ FileFilter::JSON() }, U"Assets/Stage/"))
                {
                    m_stageEditorService.load(*path);
                    
                    // 同じフォルダの敵ファイルも自動的にロード
                    FilePath enemyPath = FileSystem::ParentPath(*path) + U"../Enemy/" + FileSystem::FileName(*path);
                    if (FileSystem::Exists(enemyPath))
                    {
                        m_enemyEditorService.load(enemyPath);
                    }
                }
            }
        }
        
        void updateEnemyEditor()
        {
            if (KeyControl.pressed() && KeyP.down())
            {
                m_stageEditorService.save(U"Assets/Stage/stage_edited.json");
                m_enemyEditorService.save(U"Assets/Enemy/enemy_edited.json");
                
                auto& core = Jam::Foundation::CoreManager::Instance();
                core.stageInfo.stageName = Jam::Foundation::StageName::Stage1_1;
                
                changeScene(ToSceneString(SceneName::InGame));
                return;
            }
            
            m_enemyEditorService.updateCamera();
            
            // Alt押下中はモード切り替えしない（Alt+Tabなどのシステムショートカット対策）
            if (!KeyAlt.pressed())
            {
                if (Key1.down()) m_enemyEditorService.setMode(UseCase::Editor::EditorMode::Select);
                if (Key2.down()) m_enemyEditorService.setMode(UseCase::Editor::EditorMode::Place);
                if (Key3.down()) m_enemyEditorService.setMode(UseCase::Editor::EditorMode::Delete);
            }
            
            bool hasMouseInput = MouseL.down() || MouseL.pressed();
            
            // パネルが折りたたまれている場合は、折りたたみボタン部分のみをGUI領域とする
            const bool isMouseOverGUI = m_enemyRenderer.isMouseOverPanel();
            
            if (hasMouseInput && !isMouseOverGUI)
            {
                Vec2 mousePos = m_enemyEditorService.screenToWorld(Cursor::Pos());
                
                switch (m_enemyEditorService.getMode())
                {
                case UseCase::Editor::EditorMode::Place:
                    if (MouseL.down())
                    {
                        m_enemyEditorService.handlePlacement(mousePos);
                    }
                    break;
                    
                case UseCase::Editor::EditorMode::Select:
                    if (MouseL.down())
                    {
                        m_enemyEditorService.handleSelection(mousePos);
                    }
                    break;
                    
                case UseCase::Editor::EditorMode::Delete:
                    if (MouseL.down() || MouseL.pressed())
                    {
                        m_enemyEditorService.handleDeletion(mousePos);
                    }
                    break;
                }
            }
            
            if (KeyControl.pressed() && KeyZ.down())
            {
                m_enemyEditorService.undo();
            }
            if (KeyControl.pressed() && KeyY.down())
            {
                m_enemyEditorService.redo();
            }
            
            // Deleteキーで選択中のオブジェクトを削除
            if (KeyDelete.down())
            {
                auto selectedIds = m_enemyEditorService.getManager().getSelectedIds();
                for (auto selectedId : selectedIds)
                {
                    m_enemyEditorService.getManager().removeObject(selectedId);
                }
            }
            
            if (KeyControl.pressed() && KeyS.down())
            {
                if (const auto path = Dialog::SaveFile({ FileFilter::JSON() }, U"Assets/Enemy/"))
                {
                    m_enemyEditorService.save(*path);
                }
            }
            if (KeyControl.pressed() && KeyShift.pressed() && KeyO.down())
            {
                // 両方のJSONファイルを開く
                loadBothJsonFiles();
            }
            else if (KeyControl.pressed() && KeyO.down())
            {
                if (const auto path = Dialog::OpenFile({ FileFilter::JSON() }, U"Assets/Enemy/"))
                {
                    m_enemyEditorService.load(*path);
                    
                    // 同じフォルダのステージファイルも自動的にロード
                    FilePath stagePath = FileSystem::ParentPath(*path) + U"../Stage/" + FileSystem::FileName(*path);
                    if (FileSystem::Exists(stagePath))
                    {
                        m_stageEditorService.load(stagePath);
                    }
                }
            }
        }
        
        void draw() const override
        {
            Scene::SetBackground(ColorF{0.1, 0.15, 0.25});
            
            // アクティブなエディタのビューとGUIパネルを描画
            if (m_editorTarget == Domain::Editor::EditorTarget::Stage)
            {
                // ステージエディタの場合、ステージエディタのカメラでグリッド・ステージ・敵を描画
                {
                    const auto& camera = m_stageEditorService.getCamera();
                    auto transformer = camera.createTransformer();
                    
                    // グリッド描画
                    drawGridForStageEditor(camera);
                    
                    // プレイヤーのスポーン位置を描画
                    drawPlayerSpawnPosition();
                    
                    // 敵を背景として描画
                    drawEnemiesAsBackground();
                    
                    // ステージを描画
                    drawStageObjects();
                    
                    // ドラッグ矩形を描画
                    if (auto dragRect = m_stageEditorService.getDragRect())
                    {
                        dragRect->draw(ColorF{0.0, 1.0, 0.0, 0.3});
                        dragRect->drawFrame(2.0, ColorF{0.0, 1.0, 0.0, 0.8});
                    }
                    
                    // 選択モードのドラッグ矩形
                    if (auto selectionRect = m_stageEditorService.getSelectionDragRect())
                    {
                        selectionRect->draw(ColorF{0.0, 0.5, 1.0, 0.2});
                        selectionRect->drawFrame(2.0, ColorF{0.0, 0.5, 1.0, 0.8});
                    }
                }
                
                m_stageRenderer.drawGUIPanel();
            }
            else
            {
                // 敵エディタの場合、敵エディタのカメラでグリッド・ステージ・敵を描画
                {
                    const auto& camera = m_enemyEditorService.getCamera();
                    auto transformer = camera.createTransformer();
                    
                    // グリッド描画
                    drawGridForEnemyEditor(camera);
                    
                    // ステージを背景として描画（カメラ変換は既に適用済み）
                    drawStageBackground();
                    
                    // 敵を描画（カメラ変換は既に適用済み）
                    drawEnemies();
                }
                
                // GUIパネルは変換なしで描画
                m_enemyRenderer.drawGUIPanel();
            }
        }
        
    private:
        void drawGridForStageEditor(const Camera2D& camera) const
        {
            const int gridSize = m_stageEditorService.getSettings().getGridSize();
            const Vec2 center = camera.getCenter();
            const double scale = camera.getScale();
            const double viewWidth = Scene::Width() / scale;
            const double viewHeight = Scene::Height() / scale;
            
            const int startX = static_cast<int>((center.x - viewWidth / 2) / gridSize) - 1;
            const int endX = static_cast<int>((center.x + viewWidth / 2) / gridSize) + 1;
            const int startY = static_cast<int>((center.y - viewHeight / 2) / gridSize) - 1;
            const int endY = static_cast<int>((center.y + viewHeight / 2) / gridSize) + 1;
            
            const ColorF gridColor{0.3, 0.3, 0.3, 0.5};
            
            for (int x = startX; x <= endX; ++x) {
                double xPos = x * gridSize;
                Line{xPos, startY * gridSize, xPos, endY * gridSize}.draw(0.5, gridColor);
            }
            
            for (int y = startY; y <= endY; ++y) {
                double yPos = y * gridSize;
                Line{startX * gridSize, yPos, endX * gridSize, yPos}.draw(0.5, gridColor);
            }
            
            // 原点の軸
            Line{0, startY * gridSize, 0, endY * gridSize}.draw(2.0, Palette::Red);
            Line{startX * gridSize, 0, endX * gridSize, 0}.draw(2.0, Palette::Green);
        }
        
        void drawGridForEnemyEditor(const Camera2D& camera) const
        {
            const int gridSize = m_enemyEditorService.getSettings().getGridSize();
            const Vec2 center = camera.getCenter();
            const double scale = camera.getScale();
            const double viewWidth = Scene::Width() / scale;
            const double viewHeight = Scene::Height() / scale;
            
            const int startX = static_cast<int>((center.x - viewWidth / 2) / gridSize) - 1;
            const int endX = static_cast<int>((center.x + viewWidth / 2) / gridSize) + 1;
            const int startY = static_cast<int>((center.y - viewHeight / 2) / gridSize) - 1;
            const int endY = static_cast<int>((center.y + viewHeight / 2) / gridSize) + 1;
            
            const ColorF gridColor{0.3, 0.3, 0.3, 0.5};
            
            for (int x = startX; x <= endX; ++x) {
                double xPos = x * gridSize;
                Line{xPos, startY * gridSize, xPos, endY * gridSize}.draw(0.5, gridColor);
            }
            
            for (int y = startY; y <= endY; ++y) {
                double yPos = y * gridSize;
                Line{startX * gridSize, yPos, endX * gridSize, yPos}.draw(0.5, gridColor);
            }
            
            // 原点の軸
            Line{0, startY * gridSize, 0, endY * gridSize}.draw(2.0, Palette::Red);
            Line{startX * gridSize, 0, endX * gridSize, 0}.draw(2.0, Palette::Green);
        }
        
        void drawStageBackground() const
        {
            const auto& objects = m_stageEditorService.getManager().getAllObjects();
            for (const auto& obj : objects)
            {
                // ゲーム実行時と同じ見た目で描画（半透明にしない）
                m_stageRenderer.drawObject(obj, false);
            }
        }
        
        void drawEnemiesAsBackground() const
        {
            const auto& enemies = m_enemyEditorService.getManager().getAllObjects();
            for (const auto& enemy : enemies)
            {
                // ゲーム実行時と同じ見た目で描画（半透明にしない）
                m_enemyRenderer.drawEnemy(enemy, false);
            }
        }
        
        void drawPlayerSpawnPosition() const
        {
            const Vec2 playerSpawnPos{0, 0};
            const double playerRadius = 20.0;
            Circle{playerSpawnPos, playerRadius}.draw(ColorF{0.0, 1.0, 1.0, 0.5});
            Circle{playerSpawnPos, playerRadius}.drawFrame(2.0, ColorF{0.0, 1.0, 1.0});
            Circle{playerSpawnPos, 5.0}.draw(ColorF{1.0, 1.0, 1.0});
        }
        
        void drawStageObjects() const
        {
            const auto& objects = m_stageEditorService.getManager().getAllObjects();
            for (const auto& obj : objects)
            {
                m_stageRenderer.drawObject(obj, obj.isSelected);
                
                // 移動床のガイドを描画
                if (obj.data.type == Domain::Stage::StageType::MovingPlatform)
                {
                    drawMovementGuide(obj.data);
                }
            }
        }
        
        void drawEnemies() const
        {
            const Vec2 playerSpawnPos{0, 0};
            const double playerRadius = 20.0;
            Circle{playerSpawnPos, playerRadius}.draw(ColorF{0.0, 1.0, 1.0, 0.5});
            Circle{playerSpawnPos, playerRadius}.drawFrame(2.0, ColorF{0.0, 1.0, 1.0});
            
            for (const auto& enemy : m_enemyEditorService.getManager().getAllObjects())
            {
                m_enemyRenderer.drawEnemy(enemy, enemy.isSelected);
                if (!enemy.data.isBoss())
                {
                    m_enemyRenderer.drawPatrolRange(enemy.data);
                }
            }
        }
        
        void drawMovementGuide(const Domain::Stage::StageObject& obj) const
        {
            const Vec2 center = obj.rect.center();
            const double distance = obj.movementDistance;
            // ループのオン/オフで色を変える（オレンジ：ループ、シアン：往復）
            const ColorF guideColor = obj.loopMovement ? ColorF{1.0, 0.5, 0.0, 0.8} : ColorF{0.0, 1.0, 1.0, 0.8};
            
            Vec2 startPos, endPos;
            
            switch (obj.movementType)
            {
            case Domain::Stage::MovementType::Horizontal:
                {
                    startPos = Vec2{center.x - distance / 2, center.y};
                    endPos = Vec2{center.x + distance / 2, center.y};
                    
                    Line{startPos, endPos}.draw(3.0, guideColor);
                    
                    // ループする場合は塗りつぶした円、しない場合は中空の円
                    if (obj.loopMovement)
                    {
                        Circle{startPos, 5.0}.draw(guideColor);
                        Circle{endPos, 5.0}.draw(guideColor);
                    }
                    else
                    {
                        Circle{startPos, 5.0}.drawFrame(2.0, guideColor);
                        Circle{endPos, 5.0}.drawFrame(2.0, guideColor);
                        // 往復を示す矢印
                        Triangle{startPos.movedBy(-8, 0), 8, 0_deg}.draw(guideColor);
                        Triangle{endPos.movedBy(8, 0), 8, 180_deg}.draw(guideColor);
                    }
                }
                break;
                
            case Domain::Stage::MovementType::Vertical:
                {
                    startPos = Vec2{center.x, center.y - distance / 2};
                    endPos = Vec2{center.x, center.y + distance / 2};
                    
                    Line{startPos, endPos}.draw(3.0, guideColor);
                    
                    // ループする場合は塗りつぶした円、しない場合は中空の円
                    if (obj.loopMovement)
                    {
                        Circle{startPos, 5.0}.draw(guideColor);
                        Circle{endPos, 5.0}.draw(guideColor);
                    }
                    else
                    {
                        Circle{startPos, 5.0}.drawFrame(2.0, guideColor);
                        Circle{endPos, 5.0}.drawFrame(2.0, guideColor);
                        // 往復を示す矢印
                        Triangle{startPos.movedBy(0, -8), 8, 270_deg}.draw(guideColor);
                        Triangle{endPos.movedBy(0, 8), 8, 90_deg}.draw(guideColor);
                    }
                }
                break;
                
            case Domain::Stage::MovementType::Circular:
                {
                    Circle{center, distance / 2}.drawFrame(3.0, guideColor);
                    Circle{center, 5.0}.draw(guideColor);
                }
                break;
                
            default:
                break;
            }
        }
    };
}
