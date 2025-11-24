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

    // ステージエディタシーン
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
        StageEditorScene(const InitData& init);
        
    private:
        // ===== 初期化 =====
        void loadBothJsonFiles();
        void preloadGameResources();
        void preloadTextures();
        
        // ===== 更新 =====
        void update() override;
        void updateStageEditor();
        void updateEnemyEditor();
        
        // ===== 描画 =====
        void draw() const override;
        
        void drawGridForStageEditor(const Camera2D& camera) const;
        void drawGridForEnemyEditor(const Camera2D& camera) const;
        void drawStageBackground() const;
        void drawEnemiesAsBackground() const;
        void drawPlayerSpawnPosition() const;
        void drawStageObjects() const;
        void drawEnemies() const;
        void drawMovementGuide(const Domain::Stage::StageObject& obj) const;
    };
}