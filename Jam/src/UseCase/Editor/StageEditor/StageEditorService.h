#pragma once
#include "../Base/EditorServiceBase.h"
#include "../../../Domain/Editor/StageEditor/StageEditorManager.h"
#include "../../../Domain/Editor/StageEditor/StageEditorTypes.h"

namespace Jam::UseCase::Editor
{
    enum class OtherObjectType
    {
        None,
        Goal,
        FlagmentMemory
    };
    
    struct EditorState
    {
        Domain::Stage::StageType stageType = Domain::Stage::StageType::Normal;
        Domain::Stage::GroundSide groundSide = Domain::Stage::GroundSide::All;
        Domain::Stage::MovementType movementType = Domain::Stage::MovementType::Horizontal;
        double movementDistance = 200.0;
        double movementSpeed = 100.0;
        bool loopMovement = true;
        double damageAmount = 10.0;
        String metadata = U"普通の床";
        String texturePath = U"Assets/Stage/normal_stage.png";  // テクスチャファイルのパス（デフォルト値を設定）
        bool autoCalculateDistance = true;  // 移動距離を自動計算するか
        OtherObjectType otherObjectType = OtherObjectType::None;  // その他のオブジェクトの種類
    };
    
    class StageEditorService : public EditorServiceBase<Domain::Editor::StageEditorManager>
    {
    private:
        Domain::Editor::StageEditorSettings m_settings;
        EditorState m_state;
        
        Optional<Vec2> m_dragStart;
        Optional<Vec2> m_currentDragPos;
        
        // 選択モード用のドラッグ
        Optional<Vec2> m_selectDragStart;
        Optional<Vec2> m_selectDragCurrent;
        
        // プレイヤースポーン位置のドラッグ
        bool m_isSpawnSelected = false;
        bool m_isDraggingSpawn = false;
        Optional<Vec2> m_spawnDragStart;
        
    public:
        StageEditorService() = default;
        
        // ===== 設定アクセス =====
        const Domain::Editor::StageEditorSettings& getSettings() const { return m_settings; }
        Domain::Editor::StageEditorSettings& getSettings() { return m_settings; }
        
        // ===== ステージタイプ設定 =====
        void setCurrentStageType(Domain::Stage::StageType type);
        
        Domain::Stage::StageType getCurrentStageType() const { return m_state.stageType; }
        Domain::Stage::GroundSide getCurrentGroundSide() const { return m_state.groundSide; }
        
        // ===== 移動/ダメージ設定 =====
        void setMovementType(Domain::Stage::MovementType type) { m_state.movementType = type; }
        void setMovementDistance(double distance);
        void setMovementSpeed(double speed);
        void setLoopMovement(bool loop) { m_state.loopMovement = loop; }
        void setDamageAmount(double amount);
        void setMetadata(const String& metadata) { m_state.metadata = metadata; }
        void setTexturePath(const String& path) { m_state.texturePath = path; }
        void setAutoCalculateDistance(bool autoCalc) { m_state.autoCalculateDistance = autoCalc; }
        
        Domain::Stage::MovementType getMovementType() const { return m_state.movementType; }
        double getMovementDistance() const { return m_state.movementDistance; }
        double getMovementSpeed() const { return m_state.movementSpeed; }
        bool getLoopMovement() const { return m_state.loopMovement; }
        double getDamageAmount() const { return m_state.damageAmount; }
        const String& getMetadata() const { return m_state.metadata; }
        const String& getTexturePath() const { return m_state.texturePath; }
        bool getAutoCalculateDistance() const { return m_state.autoCalculateDistance; }
        
        // ===== その他のオブジェクト設定 =====
        void setOtherObjectType(OtherObjectType type);
        OtherObjectType getOtherObjectType() const { return m_state.otherObjectType; }
        
        // ===== テクスチャ操作 =====
        void applyTextureToSelected(const String& texturePath);
        
        // ===== ゴール・スポーン位置 =====
        Vec2 getPlayerSpawnPosition() const { return m_manager.getPlayerSpawnPosition(); }
        void setPlayerSpawnPosition(const Vec2& pos) { m_manager.setPlayerSpawnPosition(pos); }
        
        Vec2 getGoalPosition() const { return m_manager.getGoalPosition(); }
        void setGoalPosition(const Vec2& pos) { m_manager.setGoalPosition(pos); }
        
        Vec2 getGoalSize() const { return m_manager.getGoalSize(); }
        void setGoalSize(const Vec2& size) { m_manager.setGoalSize(size); }
        
        // ===== スポーン位置選択・ドラッグ =====
        bool isSpawnSelected() const { return m_isSpawnSelected; }
        bool isDraggingSpawn() const { return m_isDraggingSpawn; }
        void selectSpawn() { m_isSpawnSelected = true; }
        void deselectSpawn() { m_isSpawnSelected = false; }
        void startDraggingSpawn(const Vec2& mousePos);
        void updateSpawnDrag(const Vec2& mousePos);
        void endSpawnDrag();
        bool isMouseOverSpawn(const Vec2& mousePos, double radius = 20.0) const;
        
        // ===== グリッドスナップ =====
        Vec2 snapToGrid(const Vec2& pos) const;
        
        // ===== ドラッグ矩形 =====
        Optional<RectF> getDragRect() const;
        Optional<RectF> getSelectionDragRect() const;  // 選択モード用
        
        // ===== 入力処理（オーバーライド） =====
        void handlePlacement(const Vec2& mousePos) override;
        void handleSelection(const Vec2& mousePos) override;
        void handleDeletion(const Vec2& mousePos) override;
        
    private:
        Domain::Stage::StageObject createStageObjectFromCurrent(const RectF& rect) const;
        void updateGroundSideForType(Domain::Stage::StageType type);
        void updateMetadataForType(Domain::Stage::StageType type);
    };
}