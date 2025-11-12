#include "StageEditorService.h"

namespace Jam::UseCase::Editor
{
    StageEditorService::StageEditorService()
    {
    }

    void StageEditorService::updateCamera()
    {
        const double speed = m_settings.getCameraSpeed();
        Vec2 movement{0, 0};
        
        if (KeyW.pressed()) movement.y -= speed;
        if (KeyS.pressed()) movement.y += speed;
        if (KeyA.pressed()) movement.x -= speed;
        if (KeyD.pressed()) movement.x += speed;
        
        if (movement.x != 0.0 || movement.y != 0.0)
        {
            Vec2 newCenter = m_camera.getCenter() + movement;
            m_camera.setCenter(newCenter);
        }
        
        if (Mouse::Wheel() > 0)
        {
            double newScale = Min(4.0, m_camera.getScale() * 1.1);
            m_camera.setScale(newScale);
        }
        else if (Mouse::Wheel() < 0)
        {
            double newScale = Max(0.1, m_camera.getScale() / 1.1);
            m_camera.setScale(newScale);
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
            Math::Round(pos.x / gridSize) * gridSize,
            Math::Round(pos.y / gridSize) * gridSize
        };
    }

    void StageEditorService::handlePlacement(const Vec2& mousePos)
    {
        const int gridSize = m_settings.getGridSize();
        Vec2 snappedPos = snapToGrid(mousePos);
        
        RectF rect;
        if (m_placementOrientation == Domain::Editor::PlacementOrientation::Horizontal)
        {
            rect = RectF{snappedPos.x, snappedPos.y, gridSize, gridSize};
        }
        else
        {
            rect = RectF{snappedPos.x, snappedPos.y, gridSize, gridSize};
        }
        
        auto obj = createStageObjectFromCurrent(rect);
        m_stageManager.addObject(obj);
    }

    void StageEditorService::handleSelection(const Vec2& mousePos)
    {
        auto id = m_stageManager.findObjectAt(mousePos);
        if (id)
        {
            m_stageManager.selectObject(*id);
        }
        else
        {
            m_stageManager.clearSelection();
        }
    }

    void StageEditorService::handleDeletion(const Vec2& mousePos)
    {
        auto id = m_stageManager.findObjectAt(mousePos);
        if (id)
        {
            m_stageManager.removeObject(*id);
        }
    }

    void StageEditorService::handleMove(const Vec2& mousePos)
    {
        auto selectedId = m_stageManager.getSelectedId();
        if (selectedId)
        {
            Vec2 snapped = snapToGrid(mousePos);
            m_stageManager.moveObject(*selectedId, snapped);
        }
    }

    void StageEditorService::startTest()
    {
        m_isTestMode = true;
    }

    void StageEditorService::stopTest()
    {
        m_isTestMode = false;
    }

    void StageEditorService::updateTest()
    {
    }

    Domain::Stage::StageObject StageEditorService::createStageObjectFromCurrent(const RectF& rect) const
    {
        Domain::Stage::StageObject obj;
        obj.rect = rect;
        obj.groundSide = m_currentGroundSide;
        obj.type = m_currentStageType;
        obj.metadata = U"editor_object";
        
        obj.movementType = m_currentMovementType;
        obj.movementDistance = m_currentMovementDistance;
        obj.movementSpeed = m_currentMovementSpeed;
        
        return obj;
    }
}