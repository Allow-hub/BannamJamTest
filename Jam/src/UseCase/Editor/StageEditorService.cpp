#include "StageEditorService.h"

namespace Jam::UseCase::Editor
{
    StageEditorService::StageEditorService()
    {
    }

    void StageEditorService::updateCamera()
    {
        if (KeyControl.pressed() || KeyAlt.pressed()) return;
        
        // TextBoxがアクティブな時はキー入力を無効化
        if (TextInput::GetEditingText()) return;
        
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
        
        // ホイール向きを反転: 上スクロールで縮小、下スクロールで拡大
        if (Mouse::Wheel() > 0)
        {
            double newScale = Max(0.1, m_camera.getScale() / 1.1);
            m_camera.setScale(newScale);
        }
        else if (Mouse::Wheel() < 0)
        {
            double newScale = Min(4.0, m_camera.getScale() * 1.1);
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
            Math::Floor(pos.x / gridSize) * gridSize,
            Math::Floor(pos.y / gridSize) * gridSize
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
        
        if (m_stageManager.hasObjectAtExactPosition(rect))
        {
            return;
        }
        
        auto obj = createStageObjectFromCurrent(rect);
        m_stageManager.addObject(obj);
    }

    void StageEditorService::handleSelection(const Vec2& mousePos)
    {
        auto id = m_stageManager.findObjectAt(mousePos);
        
        // CtrlまたはShiftが押されている場合は追加選択
        bool isAdditiveSelect = KeyControl.pressed() || KeyShift.pressed();
        
        if (id)
        {
            // Ctrlが押されている場合、すでに選択されていれば解除
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
            // 何もない場所をクリックした場合、修飾キーがなければ選択解除
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
        
        // 選択中のオブジェクトをすべて削除
        auto selectedIds = m_stageManager.getSelectedIds();
        for (auto selectedId : selectedIds)
        {
            m_stageManager.removeObject(selectedId);
        }
    }

    void StageEditorService::handleMove(const Vec2& mousePos)
    {
        auto selectedIds = m_stageManager.getSelectedIds();
        if (!selectedIds.empty())
        {
            Vec2 snapped = snapToGrid(mousePos);
            
            // 最初の選択オブジェクトを基準に相対移動
            auto selectedObjects = m_stageManager.getSelectedObjects();
            if (!selectedObjects.isEmpty())
            {
                Vec2 basePos = selectedObjects[0]->stageObject.rect.pos;
                Vec2 offset = snapped - basePos;
                
                // すべての選択オブジェクトを同じオフセットで移動
                for (const auto* obj : selectedObjects)
                {
                    Vec2 newPos = obj->stageObject.rect.pos + offset;
                    m_stageManager.moveObject(*obj->id, newPos);
                }
            }
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
        obj.metadata = m_currentMetadata;
        
        obj.movementType = m_currentMovementType;
        obj.movementDistance = m_currentMovementDistance;
        obj.movementSpeed = m_currentMovementSpeed;
        obj.loopMovement = m_currentLoopMovement;
        
        obj.damageAmount = m_currentDamageAmount;
        
        return obj;
    }
}