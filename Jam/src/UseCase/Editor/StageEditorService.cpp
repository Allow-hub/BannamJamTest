#include "StageEditorService.h"

namespace Jam::UseCase::Editor
{
    StageEditorService::StageEditorService()
    {
    }

    void StageEditorService::updateCamera()
    {
        const double speed = m_settings.getCameraSpeed();
        
        if (KeyW.pressed()) m_camera.jumpTo(m_camera.getCenter() + Vec2{0, -speed}, 0.0);
        if (KeyS.pressed()) m_camera.jumpTo(m_camera.getCenter() + Vec2{0, speed}, 0.0);
        if (KeyA.pressed()) m_camera.jumpTo(m_camera.getCenter() + Vec2{-speed, 0}, 0.0);
        if (KeyD.pressed()) m_camera.jumpTo(m_camera.getCenter() + Vec2{speed, 0}, 0.0);
        
        // ホイールでカメラ移動速度調整
        if (Mouse::Wheel() > 0)
        {
            m_settings.setCameraSpeed(Min(20.0, m_settings.getCameraSpeed() + 1.0));
        }
        else if (Mouse::Wheel() < 0)
        {
            m_settings.setCameraSpeed(Max(1.0, m_settings.getCameraSpeed() - 1.0));
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
        Vec2 snapped = snapToGrid(mousePos);
        
        if (!m_placementStart)
        {
            m_placementStart = snapped;
        }
        else
        {
            Vec2 size = snapped - *m_placementStart;
            if (Abs(size.x) > 10 && Abs(size.y) > 10)
            {
                RectF rect{*m_placementStart, Abs(size.x), Abs(size.y)};
                if (size.x < 0) rect.x += size.x;
                if (size.y < 0) rect.y += size.y;
                
                auto obj = createStageObjectFromCurrent(rect);
                m_stageManager.addObject(obj);
                
                m_placementStart.reset();
            }
        }
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
        return obj;
    }
}