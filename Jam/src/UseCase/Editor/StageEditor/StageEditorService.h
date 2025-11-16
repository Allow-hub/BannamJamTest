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
        void setCurrentStageType(Domain::Stage::StageType type)
        {
            m_state.stageType = type;
            updateGroundSideForType(type);
            updateMetadataForType(type);
        }
        
        Domain::Stage::StageType getCurrentStageType() const { return m_state.stageType; }
        Domain::Stage::GroundSide getCurrentGroundSide() const { return m_state.groundSide; }
        
        // ===== 移動/ダメージ設定 =====
        void setMovementType(Domain::Stage::MovementType type) { m_state.movementType = type; }
        void setMovementDistance(double distance)
        {
            m_state.movementDistance = distance;
            m_manager.updateSelectedObjectsMovement(distance, m_state.movementSpeed, m_state.movementType);
        }
        void setMovementSpeed(double speed)
        {
            m_state.movementSpeed = speed;
            m_manager.updateSelectedObjectsMovement(m_state.movementDistance, speed, m_state.movementType);
        }
        void setLoopMovement(bool loop) { m_state.loopMovement = loop; }
        void setDamageAmount(double amount)
        {
            m_state.damageAmount = amount;
            m_manager.updateSelectedObjectsDamage(amount);
        }
        void setMetadata(const String& metadata) { m_state.metadata = metadata; }
        
        Domain::Stage::MovementType getMovementType() const { return m_state.movementType; }
        double getMovementDistance() const { return m_state.movementDistance; }
        double getMovementSpeed() const { return m_state.movementSpeed; }
        bool getLoopMovement() const { return m_state.loopMovement; }
        double getDamageAmount() const { return m_state.damageAmount; }
        const String& getMetadata() const { return m_state.metadata; }
        
        // ===== グリッドスナップ =====
        Vec2 snapToGrid(const Vec2& pos) const
        {
            if (!m_settings.isSnapToGrid()) return pos;
            
            const int gridSize = m_settings.getGridSize();
            return Vec2{
                Math::Floor(pos.x / gridSize) * gridSize,
                Math::Floor(pos.y / gridSize) * gridSize
            };
        }
        
        // ===== ドラッグ矩形 =====
        Optional<RectF> getDragRect() const
        {
            if (!m_dragStart || !m_currentDragPos) return none;
            
            const int gridSize = m_settings.getGridSize();
            Vec2 start = *m_dragStart;
            Vec2 currentPos = *m_currentDragPos;
            
            double x = Min(start.x, currentPos.x);
            double y = Min(start.y, currentPos.y);
            double x2 = Max(start.x, currentPos.x);
            double y2 = Max(start.y, currentPos.y);
            
            double w = x2 - x + gridSize;
            double h = y2 - y + gridSize;
            
            return RectF{x, y, w, h};
        }
        
        // ===== 入力処理（オーバーライド） =====
        void handlePlacement(const Vec2& mousePos) override
        {
            const int gridSize = m_settings.getGridSize();
            Vec2 snappedPos = snapToGrid(mousePos);
            
            if (MouseL.down()) {
                m_dragStart = snappedPos;
                m_currentDragPos = snappedPos;
            }
            
            if (MouseL.pressed() && m_dragStart) {
                m_currentDragPos = snappedPos;
            }
            
            if (MouseL.up() && m_dragStart) {
                Vec2 start = *m_dragStart;
                Vec2 end = snappedPos;
                
                double x = Min(start.x, end.x);
                double y = Min(start.y, end.y);
                double x2 = Max(start.x, end.x);
                double y2 = Max(start.y, end.y);
                
                double w = x2 - x + gridSize;
                double h = y2 - y + gridSize;
                
                RectF rect{x, y, w, h};
                
                // 重複配置を防ぐ：既存オブジェクトと重なっていないかチェック
                if (!m_manager.hasOverlappingObject(rect)) {
                    auto obj = createStageObjectFromCurrent(rect);
                    
                    // 動く床の場合、矩形サイズに基づいて初期距離を提案
                    if (obj.type == Domain::Stage::StageType::MovingPlatform || 
                        obj.type == Domain::Stage::StageType::MovingDamagePlatform)
                    {
                        double suggestedDistance = m_state.movementDistance;
                        
                        if (obj.movementType == Domain::Stage::MovementType::Horizontal)
                        {
                            suggestedDistance = w * 2.0;
                        }
                        else if (obj.movementType == Domain::Stage::MovementType::Vertical)
                        {
                            suggestedDistance = h * 2.0;
                        }
                        else if (obj.movementType == Domain::Stage::MovementType::Circular)
                        {
                            suggestedDistance = Min(w, h);
                        }
                        
                        obj.movementDistance = suggestedDistance;
                        // UIの設定値も更新して、次回配置時やスライダー操作に反映
                        m_state.movementDistance = suggestedDistance;
                    }
                    
                    m_manager.addObject(obj);
                }
                
                m_dragStart.reset();
                m_currentDragPos.reset();
            }
        }
        
        void handleSelection(const Vec2& mousePos) override
        {
            auto id = m_manager.findObjectAt(mousePos);
            bool isAdditiveSelect = KeyControl.pressed() || KeyShift.pressed();
            
            if (id) {
                if (KeyControl.pressed() && m_manager.getSelectedIds().contains(*id)) {
                    m_manager.deselectObject(*id);
                } else {
                    m_manager.selectObject(*id, isAdditiveSelect);
                }
            } else {
                if (!isAdditiveSelect) {
                    m_manager.clearSelection();
                }
            }
        }
        
        void handleDeletion(const Vec2& mousePos) override
        {
            // マウス位置のオブジェクトを削除
            if (auto id = m_manager.findObjectAt(mousePos)) {
                m_manager.removeObject(*id);
                return;
            }
            
            // 選択中のオブジェクトを削除
            auto selectedIds = m_manager.getSelectedIds();
            for (auto selectedId : selectedIds) {
                m_manager.removeObject(selectedId);
            }
        }
        
    private:
        Domain::Stage::StageObject createStageObjectFromCurrent(const RectF& rect) const
        {
            Domain::Stage::StageObject obj;
            obj.rect = rect;
            obj.groundSide = m_state.groundSide;
            obj.type = m_state.stageType;
            obj.metadata = m_state.metadata;
            obj.movementType = m_state.movementType;
            obj.movementDistance = m_state.movementDistance;
            obj.movementSpeed = m_state.movementSpeed;
            obj.loopMovement = m_state.loopMovement;
            obj.damageAmount = m_state.damageAmount;
            
            return obj;
        }
        
        void updateGroundSideForType(Domain::Stage::StageType type)
        {
            switch (type) {
            case Domain::Stage::StageType::Normal:
            case Domain::Stage::StageType::MovingPlatform:
                m_state.groundSide = Domain::Stage::GroundSide::All;
                break;
            case Domain::Stage::StageType::OneWayPlatform:
                m_state.groundSide = Domain::Stage::GroundSide::Up;
                break;
            case Domain::Stage::StageType::DamagePlatform:
            case Domain::Stage::StageType::MovingDamagePlatform:
                m_state.groundSide = Domain::Stage::GroundSide::None;
                break;
            }
        }
        
        void updateMetadataForType(Domain::Stage::StageType type)
        {
            // 既存の実装を使用
            switch (type) {
            case Domain::Stage::StageType::Normal:
                m_state.metadata = U"普通の床";
                break;
            case Domain::Stage::StageType::MovingPlatform:
                m_state.metadata = U"動く床";
                break;
            case Domain::Stage::StageType::OneWayPlatform:
                m_state.metadata = U"すり抜け床";
                break;
            case Domain::Stage::StageType::DamagePlatform:
                m_state.metadata = U"ダメージ床";
                break;
            case Domain::Stage::StageType::MovingDamagePlatform:
                m_state.metadata = U"動くダメージ床";
                break;
            }
        }
    };
}