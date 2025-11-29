#include "StageEditorScene.h"
#include "../../Foundation/CoreManager.h"
#include "../ResourceManager.h"
#include "../Editor/Utilities/EditorGridUtil.h"

namespace Jam::Presentation::Scenes
{
    #pragma region コンストラクタ

    StageEditorScene::StageEditorScene(const InitData& init)
        : IScene{ init }
    {
        // 両方のエディタに共有カメラを設定
        m_stageEditorService.setSharedCameraController(&m_sharedCamera);
        m_enemyEditorService.setSharedCameraController(&m_sharedCamera);
        
        m_stageRenderer.init(&m_stageEditorService);
        m_enemyRenderer.init(&m_enemyEditorService);
        
        // EnemyEditorRendererにStageEditorManagerへの参照を渡す
        m_enemyRenderer.setStageManager(&m_stageEditorService.getManager());
        
        // エディタ起動時にリソースを事前ロード（テストプレイ時の待ち時間短縮）
        preloadGameResources();
        
        // 起動時に両方のJSONファイルを開くダイアログを表示
        loadBothJsonFiles();
    }

    #pragma endregion

    #pragma region 初期化
    
    void StageEditorScene::loadBothJsonFiles()
    {
        // まずステージファイルを選択
        const auto stagePath = Dialog::OpenFile({ FileFilter::JSON() }, U"Assets/Stage/");
        if (!stagePath)
        {
            // キャンセルされた場合は何もしない
            return;
        }
        
        m_stageEditorService.load(*stagePath);
        
        // 読み込み後、テクスチャを事前読み込み
        preloadTextures();
        
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
    
    void StageEditorScene::preloadGameResources()
    {
        // プレイヤーアニメーションを事前ロード
        using namespace Jam::Foundation;
        
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
    }
    
    void StageEditorScene::preloadTextures()
    {
        const auto& objects = m_stageEditorService.getManager().getAllObjects();
        for (const auto& obj : objects)
        {
            if (!obj.data.texturePath.isEmpty())
            {
                const auto& texture = Jam::Presentation::TextureManager::Load(obj.data.texturePath);
                
                if (!texture)
                    assert(false && "Failed to preload texture. Check if the texture file exists.");
            }
        }
    }

    #pragma endregion

    #pragma region 更新
    
    void StageEditorScene::update()
    {
        if (KeyEscape.down())
        {
            System::Exit();
        }
        
        // Editor Target切り替え (Tab or GUI Button)
        if (KeyTab.down())
        {
            m_editorTarget = (m_editorTarget == Domain::Editor::EditorType::Stage) 
                ? Domain::Editor::EditorType::Enemy 
                : Domain::Editor::EditorType::Stage;
        }
        
        // GUIボタンからの切り替え
        if (m_editorTarget == Domain::Editor::EditorType::Stage && m_stageRenderer.isEditorSwitchRequested())
        {
            m_editorTarget = Domain::Editor::EditorType::Enemy;
            m_stageRenderer.resetEditorSwitchRequest();
        }
        else if (m_editorTarget == Domain::Editor::EditorType::Enemy && m_enemyRenderer.isEditorSwitchRequested())
        {
            m_editorTarget = Domain::Editor::EditorType::Stage;
            m_enemyRenderer.resetEditorSwitchRequest();
        }
        
        if (m_editorTarget == Domain::Editor::EditorType::Stage)
        {
            updateStageEditor();
        }
        else
        {
            updateEnemyEditor();
        }
    }

    #pragma endregion

    #pragma region 更新 - ステージエディタ
    
    void StageEditorScene::updateStageEditor()
    {
        if (KeyControl.pressed() && KeyP.down())
        {
            m_stageEditorService.save(U"Assets/Stage/stage_edited.json");
            m_enemyEditorService.save(U"Assets/Enemy/enemy_edited.json");
            
            using namespace Jam::Foundation;
            auto& core = CoreManager::Instance();
            core.stageInfo.stageName = StageName::Stage1_1;
            
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
        
        // ビュー領域をクリックしたらテキスト入力を無効化
        if (MouseL.down() && !isMouseOverGUI)
        {
            m_stageRenderer.clearTextInputFocus();
        }
        
        if (hasMouseInput && !isMouseOverGUI)
        {
            Vec2 mousePos = m_stageEditorService.screenToWorld(Cursor::Pos());
            
            // プレイヤースポーン位置のドラッグ処理（選択モードのみ）
            if (m_stageEditorService.getMode() == UseCase::Editor::EditorMode::Select)
            {
                if (MouseL.down())
                {
                    if (m_stageEditorService.isMouseOverSpawn(mousePos))
                    {
                        m_stageEditorService.startDraggingSpawn(mousePos);
                    }
                    else
                    {
                        m_stageEditorService.deselectSpawn();
                    }
                }
                
                if (MouseL.pressed() && m_stageEditorService.isDraggingSpawn())
                {
                    m_stageEditorService.updateSpawnDrag(mousePos);
                }
                
                if (MouseL.up() && m_stageEditorService.isDraggingSpawn())
                {
                    m_stageEditorService.endSpawnDrag();
                }
            }
            
            // スポーンドラッグ中でない場合のみ通常の処理を実行
            if (!m_stageEditorService.isDraggingSpawn())
            {
                switch (m_stageEditorService.getMode())
                {
                case UseCase::Editor::EditorMode::Place:
                    if (MouseL.down() || MouseL.pressed() || MouseL.up())
                    {
                        m_stageEditorService.handlePlacement(mousePos);
                        m_stageEditorService.handleSelection(mousePos);
                        // 配置後、テクスチャを事前読み込み
                        preloadTextures();
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
            auto selectedIndices = m_stageEditorService.getManager().getSelectedIndices();
            // インデックスを降順にソートして削除（後ろから削除）
            Array<size_t> indices(selectedIndices.begin(), selectedIndices.end());
            indices.rsort();
            for (auto index : indices)
            {
                m_stageEditorService.getManager().removeObject(index);
            }
        }
        
        // Ctrl+Aで全選択
        if (KeyControl.pressed() && KeyA.down())
        {
            m_stageEditorService.getManager().clearSelection();
            const auto& objects = m_stageEditorService.getManager().getAllObjects();
            for (size_t i = 0; i < objects.size(); ++i)
            {
                m_stageEditorService.getManager().selectObject(i, true);
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
                
                // 読み込み後、テクスチャを事前読み込み
                preloadTextures();
                
                // 同じフォルダの敵ファイルも自動的にロード
                FilePath enemyPath = FileSystem::ParentPath(*path) + U"../Enemy/" + FileSystem::FileName(*path);
                if (FileSystem::Exists(enemyPath))
                {
                    m_enemyEditorService.load(enemyPath);
                }
            }
        }
    }

    #pragma endregion

    #pragma region 更新 - 敵エディタ
    
    void StageEditorScene::updateEnemyEditor()
    {
        if (KeyControl.pressed() && KeyP.down())
        {
            m_stageEditorService.save(U"Assets/Stage/stage_edited.json");
            m_enemyEditorService.save(U"Assets/Enemy/enemy_edited.json");
            
            using namespace Jam::Foundation;
            auto& core = CoreManager::Instance();
            core.stageInfo.stageName = StageName::Stage1_1;
            
            changeScene(ToSceneString(SceneName::InGame));
            return;
        }
        
        // テキスト入力中でない場合のみカメラ更新
        if (!m_enemyRenderer.isTextInputActive())
        {
            m_enemyEditorService.updateCamera();
        }
        
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
            auto selectedIndices = m_enemyEditorService.getManager().getSelectedIndices();
            // インデックスを降順にソートして削除（後ろから削除）
            Array<size_t> indices(selectedIndices.begin(), selectedIndices.end());
            indices.rsort();
            for (auto index : indices)
            {
                m_enemyEditorService.getManager().removeObject(index);
            }
        }
        
        // Ctrl+Aで全選択
        if (KeyControl.pressed() && KeyA.down())
        {
            m_enemyEditorService.getManager().clearSelection();
            const auto& objects = m_enemyEditorService.getManager().getAllObjects();
            for (size_t i = 0; i < objects.size(); ++i)
            {
                m_enemyEditorService.getManager().selectObject(i, true);
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

    #pragma endregion

    #pragma region 描画
    
    void StageEditorScene::draw() const
    {
        Scene::SetBackground(ColorF{0.1, 0.15, 0.25}); // Paletteでも表現できない色のためColorFを使用
        
        // アクティブなエディタのビューとGUIパネルを描画
        if (m_editorTarget == Domain::Editor::EditorType::Stage)
        {
            // ステージエディタの場合、ステージエディタのカメラでグリッド・ステージ・敵を描画
            {
                const auto& camera = m_stageEditorService.getCamera();
                auto transformer = camera.createTransformer();
                
                // グリッド描画 - EditorGridUtilを使用
                const int gridSize = m_stageEditorService.getSettings().getGridSize();
                Editor::EditorGridUtil::drawGridWithAxes(camera, gridSize);
                
                // プレイヤーのスポーン位置を描画
                drawPlayerSpawnPosition();
                
                // 敵を背景として描画
                drawEnemiesAsBackground();
                
                // ステージを描画
                drawStageObjects();   
            }
            
            // 選択モードのドラッグ矩形をスクリーン座標で描画
            if (auto selectionRect = m_stageEditorService.getSelectionDragRect())
            {
                // Camera2Dを使用してワールド座標をスクリーン座標に変換
                const auto& camera = m_stageEditorService.getCamera();
                const Mat3x2 transform = camera.getMat3x2();
                
                Vec2 topLeft = transform.transformPoint(selectionRect->pos);
                Vec2 bottomRight = transform.transformPoint(selectionRect->pos + selectionRect->size);
                RectF screenRect = RectF{topLeft, bottomRight - topLeft};
                
				// Paletteでも表現できない色のためColorFを使用
                screenRect.draw(ColorF{0.0, 0.5, 1.0, 0.2}); 
                screenRect.drawFrame(2.0, ColorF{0.0, 0.5, 1.0, 0.8});
            }
            
            m_stageRenderer.drawGUIPanel();
        }
        else
        {
            // 敵エディタの場合、敵エディタのカメラでグリッド・ステージ・敵を描画
            {
                const auto& camera = m_enemyEditorService.getCamera();
                auto transformer = camera.createTransformer();
                
                // グリッド描画 - EditorGridUtilを使用
                const int gridSize = m_enemyEditorService.getSettings().getGridSize();
                Editor::EditorGridUtil::drawGridWithAxes(camera, gridSize);
                
                // ステージを背景として描画
                drawStageBackground();
                
                // 敵を描画
                drawEnemies();
            }
            
            // GUIパネルは変換なしで描画
            m_enemyRenderer.drawGUIPanel();
        }
    }

    #pragma endregion

    #pragma region 描画 - ヘルパーメソッド
    
    void StageEditorScene::drawStageBackground() const
    {
        const auto& objects = m_stageEditorService.getManager().getAllObjects();
        for (const auto& obj : objects)
        {
            // ゲーム実行時と同じ見た目で描画（半透明にしない）
            m_stageRenderer.drawObject(obj, false);
        }
    }
    
    void StageEditorScene::drawEnemiesAsBackground() const
    {
        const auto& enemies = m_enemyEditorService.getManager().getAllObjects();
        for (const auto& enemy : enemies)
        {
            // ゲーム実行時と同じ見た目で描画（半透明にしない）
            m_enemyRenderer.drawEnemy(enemy, false);
        }
    }
    
    void StageEditorScene::drawPlayerSpawnPosition() const
    {
        const Vec2 playerSpawnPos{0, 0};
        const double playerRadius = 20.0;
        Circle{playerSpawnPos, playerRadius}.draw(Palette::Cyan.withAlpha(0.5));
        Circle{playerSpawnPos, playerRadius}.drawFrame(2.0, Palette::Cyan);
        Circle{playerSpawnPos, 5.0}.draw(Palette::White);
    }
    
    void StageEditorScene::drawStageObjects() const
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
    
    void StageEditorScene::drawEnemies() const
    {
        const Vec2 playerSpawnPos{0, 0};
        const double playerRadius = 20.0;
        Circle{playerSpawnPos, playerRadius}.draw(Palette::Cyan.withAlpha(0.5));
        Circle{playerSpawnPos, playerRadius}.drawFrame(2.0, Palette::Cyan);
        
        for (const auto& enemy : m_enemyEditorService.getManager().getAllObjects())
        {
            m_enemyRenderer.drawEnemy(enemy, enemy.isSelected);
            if (!enemy.data.isBoss())
            {
                m_enemyRenderer.drawPatrolRange(enemy.data);
            }
        }
    }
    
    void StageEditorScene::drawMovementGuide(const Domain::Stage::StageObject& obj) const
    {
        static constexpr double GUIDE_LINE_WIDTH = 3.0;
        static constexpr double GUIDE_POINT_RADIUS = 5.0;
        static constexpr double GUIDE_POINT_FRAME_WIDTH = 2.0;
        static constexpr double GUIDE_ARROW_SIZE = 8.0;
        
        const Vec2 center = obj.rect.center();
        const double distance = obj.movementDistance;
        // ループのオン/オフで色を変える（オレンジ：ループ、シアン：往復）
        const ColorF guideColor = obj.loopMovement ? Palette::Orange : Palette::Cyan;
        
        Vec2 startPos, endPos;
        
        switch (obj.movementType)
        {
        case Domain::Stage::MovementType::Horizontal:
            {
                const double halfWidth = obj.rect.w / 2.0;
                startPos = Vec2{center.x - halfWidth, center.y};
                endPos = Vec2{center.x + distance + halfWidth, center.y};
                
                Line{startPos, endPos}.draw(GUIDE_LINE_WIDTH, guideColor);
                
                // ループする場合は塗りつぶした円、しない場合は中空の円
                if (obj.loopMovement)
                {
                    Circle{startPos, GUIDE_POINT_RADIUS}.draw(guideColor);
                    Circle{endPos, GUIDE_POINT_RADIUS}.draw(guideColor);
                }
                else
                {
                    Circle{startPos, GUIDE_POINT_RADIUS}.drawFrame(GUIDE_POINT_FRAME_WIDTH, guideColor);
                    Circle{endPos, GUIDE_POINT_RADIUS}.drawFrame(GUIDE_POINT_FRAME_WIDTH, guideColor);
                    Vec2 bedStart = Vec2{center.x, center.y};
                    Vec2 bedEnd = Vec2{center.x + distance, center.y};
                    Triangle{bedStart.movedBy(-GUIDE_ARROW_SIZE, 0), GUIDE_ARROW_SIZE, 0_deg}.draw(guideColor);
                    Triangle{bedEnd.movedBy(GUIDE_ARROW_SIZE, 0), GUIDE_ARROW_SIZE, 180_deg}.draw(guideColor);
                }
            }
            break;
            
        case Domain::Stage::MovementType::Vertical:
            {
                const double halfHeight = obj.rect.h / 2.0;
                startPos = Vec2{center.x, center.y - halfHeight};
                endPos = Vec2{center.x, center.y + distance + halfHeight};
                
                Line{startPos, endPos}.draw(GUIDE_LINE_WIDTH, guideColor);
                
                // ループする場合は塗りつぶした円、しない場合は中空の円
                if (obj.loopMovement)
                {
                    Circle{startPos, GUIDE_POINT_RADIUS}.draw(guideColor);
                    Circle{endPos, GUIDE_POINT_RADIUS}.draw(guideColor);
                }
                else
                {
                    Circle{startPos, GUIDE_POINT_RADIUS}.drawFrame(GUIDE_POINT_FRAME_WIDTH, guideColor);
                    Circle{endPos, GUIDE_POINT_RADIUS}.drawFrame(GUIDE_POINT_FRAME_WIDTH, guideColor);
                    Vec2 bedStartV = Vec2{center.x, center.y};
                    Vec2 bedEndV = Vec2{center.x, center.y + distance};
                    Triangle{bedStartV.movedBy(0, -GUIDE_ARROW_SIZE), GUIDE_ARROW_SIZE, 270_deg}.draw(guideColor);
                    Triangle{bedEndV.movedBy(0, GUIDE_ARROW_SIZE), GUIDE_ARROW_SIZE, 90_deg}.draw(guideColor);
                }
            }
            break;
            
        case Domain::Stage::MovementType::Circular:
            {
                const double maxObjectRadius = Math::Max(obj.rect.w, obj.rect.h) / 2.0;
                const double adjustedRadius = distance + maxObjectRadius;
                Circle{center, adjustedRadius}.drawFrame(GUIDE_LINE_WIDTH, guideColor);
                Circle{center, GUIDE_POINT_RADIUS}.draw(guideColor);
            }
            break;
            
        default:
            break;
        }
    }

    #pragma endregion
}