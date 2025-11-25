#include "StageEditorService.h"

namespace Jam::UseCase::Editor
{
    #pragma region 状態設定

    void StageEditorService::setCurrentStageType(Domain::Stage::StageType type)
    {
        m_state.stageType = type;
        m_state.otherObjectType = OtherObjectType::None;  // ステージタイプを選択したらその他オブジェクトの選択を解除
        updateGroundSideForType(type);
        
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
        
        // その他オブジェクトが選択されていない場合は早期リターン
        if (type == OtherObjectType::None) return;
        
        // その他オブジェクトを選択したらステージタイプをNormalに戻す（配置用の一時的な設定）
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

    #pragma endregion

    #pragma region 座標変換

    Vec2 StageEditorService::snapToGrid(const Vec2& pos) const
    {
        if (!m_settings.isSnapToGrid()) return pos;
        
        const int gridSize = m_settings.getGridSize();
        return Vec2{
            Math::Floor(pos.x / gridSize) * gridSize,
            Math::Floor(pos.y / gridSize) * gridSize
        };
    }

    #pragma endregion

    #pragma region ドラッグ処理

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

    #pragma endregion

    #pragma region 配置・選択・削除処理

    void StageEditorService::handlePlacement(const Vec2& mousePos)
    {
        const int gridSize = m_settings.getGridSize();
        Vec2 snappedPos = snapToGrid(mousePos);
        
        // ゴールまたは記憶のかけらの場合はクリックで固定サイズ配置
        if (m_state.otherObjectType == OtherObjectType::Goal || 
            m_state.otherObjectType == OtherObjectType::FlagmentMemory)
        {
            handleOtherObjectPlacement(snappedPos);
            return;
        }
        
        // 通常のステージオブジェクトの場合はドラッグで配置
        handleNormalObjectPlacement(snappedPos, gridSize);
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
            handleSelectionDragEnd(mousePos);
            m_selectDragStart.reset();
            m_selectDragCurrent.reset();
        }
    }
    
    void StageEditorService::handleDeletion(const Vec2& mousePos)
    {
        if (auto id = m_manager.findObjectAt(mousePos))
        {
            m_manager.removeObject(*id);
            return;
        }
        
        auto selectedIndices = m_manager.getSelectedIndices();
        // インデックスを降順にソートして後ろから削除
        Array<size_t> indices(selectedIndices.begin(), selectedIndices.end());
        indices.rsort();
        for (auto index : indices)
            m_manager.removeObject(index);
    }

    #pragma endregion

    #pragma region オブジェクト生成

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
    


    #pragma endregion

    #pragma region スポーン位置ドラッグ処理

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

    #pragma endregion

    #pragma region 配置ヘルパー関数

    // ゴールや記憶のかけらなどのその他オブジェクトの配置処理
    void StageEditorService::handleOtherObjectPlacement(const Vec2& snappedPos)
    {
        if (!MouseL.down()) return;
        
        static constexpr double GOAL_SIZE = 200.0;
        static constexpr double FLAGMENT_SIZE = 150.0;
        
        RectF rect;
        if (m_state.otherObjectType == OtherObjectType::Goal)
        
            rect = RectF{snappedPos.x - GOAL_SIZE / 2, snappedPos.y - GOAL_SIZE / 2, GOAL_SIZE, GOAL_SIZE};
        else
            rect = RectF{snappedPos.x - FLAGMENT_SIZE / 2, snappedPos.y - FLAGMENT_SIZE / 2, FLAGMENT_SIZE, FLAGMENT_SIZE};
        
        if (!m_manager.hasOverlappingObject(rect))
        {
            auto obj = createStageObjectFromCurrent(rect);
            
            // 記憶のかけらの配置数制限
            if (m_state.otherObjectType == OtherObjectType::FlagmentMemory)
                limitFlagmentMemoryCount();
            
            m_manager.addObject(obj);
        }
    }

    // 記憶のかけらの数を制限（最大3つまで）
    void StageEditorService::limitFlagmentMemoryCount()
    {
        static constexpr size_t MAX_FLAGMENTS = 3;
        
        Array<size_t> flagmentIndices;
        const auto& objects = m_manager.getAllObjects();
        for (size_t i = 0; i < objects.size(); ++i)
        {
            if (objects[i].data.metadata == U"flagment")
                flagmentIndices.push_back(i);
        }
        
        if (flagmentIndices.size() >= MAX_FLAGMENTS)
            m_manager.removeObject(flagmentIndices[0]);
    }

    // 通常のステージオブジェクトの配置処理（ドラッグでサイズ決定）
    void StageEditorService::handleNormalObjectPlacement(const Vec2& snappedPos, int gridSize)
    {
        if (MouseL.down())
        {
            m_dragStart = snappedPos;
            m_currentDragPos = snappedPos;
        }
        
        if (MouseL.pressed() && m_dragStart)
            m_currentDragPos = snappedPos;
        
        if (MouseL.up() && m_dragStart)
            finalizePlacementDrag(snappedPos, gridSize);
    }

    // ドラッグ終了時の配置処理
    void StageEditorService::finalizePlacementDrag(const Vec2& snappedPos, int gridSize)
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
        
        if (!m_manager.hasOverlappingObject(rect))
        {
            auto obj = createStageObjectFromCurrent(rect);
            applyAutoMovementDistance(obj, w, h);
            m_manager.addObject(obj);
        }
        
        m_dragStart.reset();
        m_currentDragPos.reset();
    }

    // 移動床の移動距離を自動計算
    void StageEditorService::applyAutoMovementDistance(Domain::Stage::StageObject& obj, double width, double height)
    {
        if (!m_state.autoCalculateDistance) return;
        if (obj.type != Domain::Stage::StageType::MovingPlatform && 
            obj.type != Domain::Stage::StageType::MovingDamagePlatform) return;
        
        static constexpr double MOVEMENT_DISTANCE_MULTIPLIER = 2.0;
        double suggestedDistance = m_state.movementDistance;
        
        if (obj.movementType == Domain::Stage::MovementType::Horizontal)
            suggestedDistance = width * MOVEMENT_DISTANCE_MULTIPLIER;
        else if (obj.movementType == Domain::Stage::MovementType::Vertical)
            suggestedDistance = height * MOVEMENT_DISTANCE_MULTIPLIER;
        else if (obj.movementType == Domain::Stage::MovementType::Circular)
            suggestedDistance = Max(width, height);
        
        obj.movementDistance = suggestedDistance;
        m_state.movementDistance = suggestedDistance;
    }

    // 選択ドラッグ終了時の処理
    void StageEditorService::handleSelectionDragEnd(const Vec2& mousePos)
    {
        static constexpr double CLICK_THRESHOLD = 5.0;
        
        const bool isAdditiveSelect = KeyControl.pressed() || KeyShift.pressed();
        const double dragDistance = m_selectDragStart->distanceFrom(mousePos);
        
        if (dragDistance < CLICK_THRESHOLD)
            handleClickSelection(mousePos, isAdditiveSelect);
        else
            handleRectSelection(isAdditiveSelect);
    }
    
    // クリック選択の処理
    void StageEditorService::handleClickSelection(const Vec2& mousePos, bool isAdditiveSelect)
    {
        auto index = m_manager.findObjectAt(mousePos);
        
        if (index)
        {
            if (KeyControl.pressed() && m_manager.getSelectedIndices().contains(*index))
                m_manager.deselectObject(*index);
            else
                m_manager.selectObject(*index, isAdditiveSelect);
        }
        else
        {
            if (!isAdditiveSelect)
                m_manager.clearSelection();
        }
    }
    
    // 矩形選択の処理
    void StageEditorService::handleRectSelection(bool isAdditiveSelect)
    {
        auto rect = getSelectionDragRect();
        if (!rect) return;
        
        if (!isAdditiveSelect)
            m_manager.clearSelection();
        
        const auto& objects = m_manager.getAllObjects();
        for (size_t i = 0; i < objects.size(); ++i)
        {
            if (rect->intersects(objects[i].data.rect))
                m_manager.selectObject(i, true);
        }
    }

    #pragma endregion
}
