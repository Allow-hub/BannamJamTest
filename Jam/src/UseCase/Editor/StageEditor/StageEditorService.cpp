#include "StageEditorService.h"

namespace Jam::UseCase::Editor
{
    StageEditorService::StageEditorService()
    {
    }

    void StageEditorService::updateCamera()
    {
        if (KeyControl.pressed() || KeyAlt.pressed() || TextInput::GetEditingText()) return;
        
        const double speed = m_settings.getCameraSpeed();
        Vec2 movement{0, 0};
        
        if (KeyW.pressed()) movement.y -= speed;
        if (KeyS.pressed()) movement.y += speed;
        if (KeyA.pressed()) movement.x -= speed;
        if (KeyD.pressed()) movement.x += speed;
        
        if (movement.x != 0.0 || movement.y != 0.0)
        {
            m_camera.setCenter(m_camera.getCenter() + movement);
        }
        
        if (Mouse::Wheel() > 0)
        {
            m_camera.setScale(Max(0.1, m_camera.getScale() / 1.1));
        }
        else if (Mouse::Wheel() < 0)
        {
            m_camera.setScale(Min(4.0, m_camera.getScale() * 1.1));
        }
    }

    Vec2 StageEditorService::screenToWorld(const Vec2& screenPos) const
    {
        const Vec2 screenCenter(Scene::Width() * 0.5, Scene::Height() * 0.5);
        return m_camera.getCenter() + (screenPos - screenCenter) / m_camera.getScale();
    }

    Vec2 StageEditorService::snapToGrid(const Vec2& pos) const
    {
        if (!m_settings.isSnapToGrid()) return pos;
        
        const int gridSize = m_settings.getGridSize();
        return Vec2{
            Math::Floor(pos.x / gridSize) * gridSize,
            Math::Floor(pos.y / gridSize) * gridSize
        };
    }

    void StageEditorService::setCurrentStageType(Domain::Stage::StageType type)
    {
        m_state.stageType = type;
        updateGroundSideForType(type);
        updateMetadataForType(type);
    }

    void StageEditorService::updateGroundSideForType(Domain::Stage::StageType type)
    {
        switch (type)
        {
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

    void StageEditorService::updateMetadataForType(Domain::Stage::StageType type)
    {
        switch (type)
        {
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

    void StageEditorService::setMovementDistance(double distance)
    {
        m_state.movementDistance = distance;
        m_stageManager.updateSelectedObjectsMovement(distance, m_state.movementSpeed, m_state.movementType);
    }

    void StageEditorService::setMovementSpeed(double speed)
    {
        m_state.movementSpeed = speed;
        m_stageManager.updateSelectedObjectsMovement(m_state.movementDistance, speed, m_state.movementType);
    }

    void StageEditorService::setDamageAmount(double amount)
    {
        m_state.damageAmount = amount;
        m_stageManager.updateSelectedObjectsDamage(amount);
    }

    void StageEditorService::handlePlacement(const Vec2& mousePos)
    {
        const int gridSize = m_settings.getGridSize();
        Vec2 snappedPos = snapToGrid(mousePos);
        
        if (MouseL.down())
        {
            m_dragStart = snappedPos;
            m_currentDragPos = snappedPos;
        }
        
        if (MouseL.pressed() && m_dragStart)
        {
            m_currentDragPos = snappedPos;
        }
        
        if (MouseL.up() && m_dragStart)
        {
            Vec2 start = *m_dragStart;
            Vec2 end = snappedPos;
            
            double x = Min(start.x, end.x);
            double y = Min(start.y, end.y);
            double x2 = Max(start.x, end.x);
            double y2 = Max(start.y, end.y);
            
            double w = x2 - x + gridSize;
            double h = y2 - y + gridSize;
            
            RectF rect{x, y, w, h};
            
            if (!m_stageManager.hasObjectAtExactPosition(rect))
            {
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
                
                m_stageManager.addObject(obj);
            }
            m_dragStart.reset();
            m_currentDragPos.reset();
        }
    }
    
    Optional<RectF> StageEditorService::getDragRect() const
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

    void StageEditorService::handleSelection(const Vec2& mousePos)
    {
        auto id = m_stageManager.findObjectAt(mousePos);
        
        bool isAdditiveSelect = KeyControl.pressed() || KeyShift.pressed();
        
        if (id)
        {
            if (KeyControl.pressed() && m_stageManager.getSelectedIds().contains(*id))
            {
                m_stageManager.deselectObject(*id);
            }
            else
            {
                m_stageManager.selectObject(*id, isAdditiveSelect);
            }
        }
        else
        {
            if (!isAdditiveSelect)
            {
                m_stageManager.clearSelection();
            }
        }
    }

    void StageEditorService::handleDeletion(const Vec2& mousePos)
    {
        auto id = m_stageManager.findObjectAt(mousePos);
        if (id)
        {
            m_stageManager.removeObject(*id);
        }
        
        auto selectedIds = m_stageManager.getSelectedIds();
        for (auto selectedId : selectedIds)
        {
            m_stageManager.removeObject(selectedId);
        }
    }

    Domain::Stage::StageObject StageEditorService::createStageObjectFromCurrent(const RectF& rect) const
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
}
