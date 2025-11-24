#include "StageEditorService.h"

namespace Jam::UseCase::Editor
{
    void StageEditorService::setCurrentStageType(Domain::Stage::StageType type)
    {
        m_state.stageType = type;
        m_state.otherObjectType = OtherObjectType::None;  // ステージタイプを選択したらその他オブジェクトの選択を解除
        updateGroundSideForType(type);
        updateMetadataForType(type);
        
        // ステージタイプに応じてデフォルトテクスチャを設定
        switch (type)
        {
        case Domain::Stage::StageType::Normal:
        case Domain::Stage::StageType::OneWayPlatform:
            m_state.texturePath = U"Assets/Stage/normal_stage.png";
            break;
        case Domain::Stage::StageType::MovingPlatform:
            m_state.texturePath = U"Assets/Stage/moving_platform.png";
            break;
        case Domain::Stage::StageType::DamagePlatform:
        case Domain::Stage::StageType::MovingDamagePlatform:
            m_state.texturePath = U"Assets/Stage/damage_Stage.jpg";
            break;
        default:
            m_state.texturePath = U"Assets/Stage/normal_stage.png";
            break;
        }
    }
    
    void StageEditorService::setOtherObjectType(OtherObjectType type)
    {
        m_state.otherObjectType = type;
        
        // その他オブジェクトを選択したらステージタイプをNormalに戻す（配置用の一時的な設定）
        if (type != OtherObjectType::None)
        {
            m_state.stageType = Domain::Stage::StageType::Normal;
            m_state.groundSide = Domain::Stage::GroundSide::None;
            
            if (type == OtherObjectType::Goal)
            {
                m_state.metadata = U"goal";
                m_state.texturePath = U"Assets/Stage/goal.png";
            }
            else if (type == OtherObjectType::FlagmentMemory)
            {
                m_state.metadata = U"flagment";
                m_state.texturePath = U"Assets/FlagmentMemory.png";
            }
        }
    }
    
    void StageEditorService::setMovementDistance(double distance)
    {
        m_state.movementDistance = distance;
        m_manager.updateSelectedObjectsMovement(distance, m_state.movementSpeed, m_state.movementType);
    }
    
    void StageEditorService::setMovementSpeed(double speed)
    {
        m_state.movementSpeed = speed;
        m_manager.updateSelectedObjectsMovement(m_state.movementDistance, speed, m_state.movementType);
    }
    
    void StageEditorService::setDamageAmount(double amount)
    {
        m_state.damageAmount = amount;
        m_manager.updateSelectedObjectsDamage(amount);
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
    
    Optional<RectF> StageEditorService::getSelectionDragRect() const
    {
        if (!m_selectDragStart || !m_selectDragCurrent) return none;
        
        Vec2 start = *m_selectDragStart;
        Vec2 current = *m_selectDragCurrent;
        
        double x = Min(start.x, current.x);
        double y = Min(start.y, current.y);
        double w = Abs(current.x - start.x);
        double h = Abs(current.y - start.y);
        
        return RectF{x, y, w, h};
    }
    
    void StageEditorService::handlePlacement(const Vec2& mousePos)
    {
        const int gridSize = m_settings.getGridSize();
        Vec2 snappedPos = snapToGrid(mousePos);
        
        // ゴールまたは記憶のかけらの場合はクリックで固定サイズ配置
        if (m_state.otherObjectType == OtherObjectType::Goal || 
            m_state.otherObjectType == OtherObjectType::FlagmentMemory)
        {
            if (MouseL.down())
            {
                // 固定サイズで配置
                RectF rect;
                if (m_state.otherObjectType == OtherObjectType::Goal)
                {
                    // ゴールのサイズ: 200x200
                    rect = RectF{snappedPos.x - 100, snappedPos.y - 100, 200, 200};
                }
                else // FlagmentMemory
                {
                    // 記憶のかけらのサイズ: 150x150
                    rect = RectF{snappedPos.x - 75, snappedPos.y - 75, 150, 150};
                }
                
                if (!m_manager.hasOverlappingObject(rect))
                {
                    auto obj = createStageObjectFromCurrent(rect);
                    
                    // 記憶のかけらの配置数制限（3つまで）
                    if (m_state.otherObjectType == OtherObjectType::FlagmentMemory)
                    {
                        // 既存の記憶のかけらを検索
                        Array<size_t> flagmentIndices;
                        const auto& objects = m_manager.getAllObjects();
                        for (size_t i = 0; i < objects.size(); ++i)
                        {
                            if (objects[i].data.metadata == U"flagment")
                            {
                                flagmentIndices.push_back(i);
                            }
                        }
                        
                        // 3つ以上ある場合は最も古い（最初の）ものを削除
                        if (flagmentIndices.size() >= 3)
                        {
                            m_manager.removeObject(flagmentIndices[0]);
                        }
                    }
                    
                    m_manager.addObject(obj);
                }
            }
            return;
        }
        
        // 通常のステージオブジェクトの場合はドラッグで配置
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
            
            if (!m_manager.hasOverlappingObject(rect)) {
                auto obj = createStageObjectFromCurrent(rect);
                
                // 自動計算モードの場合のみ、サイズに基づいて距離を設定
                if (m_state.autoCalculateDistance &&
                    (obj.type == Domain::Stage::StageType::MovingPlatform || 
                     obj.type == Domain::Stage::StageType::MovingDamagePlatform))
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
                        // 円運動の半径：床の長辺を基準とする
                        suggestedDistance = Max(w, h);
                    }
                    
                    obj.movementDistance = suggestedDistance;
                    m_state.movementDistance = suggestedDistance;
                }
                
                m_manager.addObject(obj);
            }
            
            m_dragStart.reset();
            m_currentDragPos.reset();
        }
    }
    
    void StageEditorService::handleSelection(const Vec2& mousePos)
    {
        // ドラッグ開始
        if (MouseL.down())
        {
            m_selectDragStart = mousePos;
            m_selectDragCurrent = mousePos;
        }
        
        // ドラッグ中
        if (MouseL.pressed() && m_selectDragStart)
        {
            m_selectDragCurrent = mousePos;
        }
        
        // ドラッグ終了
        if (MouseL.up() && m_selectDragStart)
        {
            const bool isAdditiveSelect = KeyControl.pressed() || KeyShift.pressed();
            
            // ドラッグが小さい場合はクリックとみなす
            const double dragDistance = m_selectDragStart->distanceFrom(mousePos);
            if (dragDistance < 5.0)
            {
                // クリック選択
                auto index = m_manager.findObjectAt(mousePos);
                
                if (index) {
                    if (KeyControl.pressed() && m_manager.getSelectedIndices().contains(*index)) {
                        m_manager.deselectObject(*index);
                    } else {
                        m_manager.selectObject(*index, isAdditiveSelect);
                    }
                } else {
                    if (!isAdditiveSelect) {
                        m_manager.clearSelection();
                    }
                }
            }
            else
            {
                // 矩形選択
                if (auto rect = getSelectionDragRect())
                {
                    if (!isAdditiveSelect)
                    {
                        m_manager.clearSelection();
                    }
                    
                    // 矩形内のオブジェクトをすべて選択
                    const auto& objects = m_manager.getAllObjects();
                    for (size_t i = 0; i < objects.size(); ++i)
                    {
                        if (rect->intersects(objects[i].data.rect))
                        {
                            m_manager.selectObject(i, true);
                        }
                    }
                }
            }
            
            m_selectDragStart.reset();
            m_selectDragCurrent.reset();
        }
    }
    
    void StageEditorService::handleDeletion(const Vec2& mousePos)
    {
        if (auto id = m_manager.findObjectAt(mousePos)) {
            m_manager.removeObject(*id);
            return;
        }
        
        auto selectedIndices = m_manager.getSelectedIndices();
        // インデックスを降順にソートして後ろから削除
        Array<size_t> indices(selectedIndices.begin(), selectedIndices.end());
        indices.rsort();
        for (auto index : indices) {
            m_manager.removeObject(index);
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
        obj.texturePath = m_state.texturePath;  // テクスチャパスを設定
        
        return obj;
    }
    
    void StageEditorService::updateGroundSideForType(Domain::Stage::StageType type)
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
    
    void StageEditorService::updateMetadataForType(Domain::Stage::StageType type)
    {
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
    
    // ===== スポーン位置のドラッグ処理 =====
    
    bool StageEditorService::isMouseOverSpawn(const Vec2& mousePos, double radius) const
    {
        const Vec2 spawnPos = getPlayerSpawnPosition();
        return Circle{spawnPos, radius}.contains(mousePos);
    }
    
    void StageEditorService::startDraggingSpawn(const Vec2& mousePos)
    {
        if (isMouseOverSpawn(mousePos))
        {
            m_isDraggingSpawn = true;
            m_isSpawnSelected = true;
            m_spawnDragStart = mousePos;
        }
    }
    
    void StageEditorService::updateSpawnDrag(const Vec2& mousePos)
    {
        if (m_isDraggingSpawn && m_spawnDragStart)
        {
            Vec2 offset = mousePos - *m_spawnDragStart;
            Vec2 newPos = getPlayerSpawnPosition() + offset;
            
            // グリッドスナップ
            if (m_settings.isSnapToGrid())
            {
                newPos = snapToGrid(newPos);
            }
            
            setPlayerSpawnPosition(newPos);
            m_spawnDragStart = mousePos;
        }
    }
    
    void StageEditorService::endSpawnDrag()
    {
        m_isDraggingSpawn = false;
        m_spawnDragStart.reset();
    }
    
    void StageEditorService::applyTextureToSelected(const String& texturePath)
    {
        const auto& selectedIndices = m_manager.getSelectedIndices();
        const auto& objects = m_manager.getAllObjects();
        for (size_t i = 0; i < objects.size(); ++i)
        {
            if (selectedIndices.contains(i))
            {
                auto newObj = objects[i].data;
                newObj.texturePath = texturePath;
                m_manager.modifyObject(i, newObj);
            }
        }
    }
}
