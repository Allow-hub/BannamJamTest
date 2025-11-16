// ========================================
// StageEditorService.h（リファクタリング版）
// ========================================
#pragma once
#include "../Base/EditorServiceBase.h"
#include "../../../Domain/Editor/StageEditor/StageEditorManager.h"
#include "../../../Domain/Editor/StageEditor/StageEditorTypes.h"

namespace Jam::UseCase::Editor
{
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
    };
    
    class StageEditorService : public EditorServiceBase<Domain::Editor::StageEditorManager>
    {
    private:
        Domain::Editor::StageEditorSettings m_settings;
        EditorState m_state;
        
        Optional<Vec2> m_dragStart;
        Optional<Vec2> m_currentDragPos;
        
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
        
        Domain::Stage::MovementType getMovementType() const { return m_state.movementType; }
        double getMovementDistance() const { return m_state.movementDistance; }
        double getMovementSpeed() const { return m_state.movementSpeed; }
        bool getLoopMovement() const { return m_state.loopMovement; }
        double getDamageAmount() const { return m_state.damageAmount; }
        const String& getMetadata() const { return m_state.metadata; }
        
        // ===== グリッドスナップ =====
        Vec2 snapToGrid(const Vec2& pos) const;
        
        // ===== ドラッグ矩形 =====
        Optional<RectF> getDragRect() const;
        
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