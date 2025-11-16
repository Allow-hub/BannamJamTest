// ========================================
// StageEditorManager.h（リファクタリング版 - Modify改善）
// ========================================
#pragma once
#include "../Base/EditorManagerBase.h"
#include "StageEditorTypes.h"
#include "../../Stage/StageTypes.h"

namespace Jam::Domain::Editor
{
    // ステージ用のエディタオブジェクト（新基底クラス対応）
    struct StageEditorObjectNew : EditorObjectBase<Stage::StageObject>
    {
        using DataType = Stage::StageObject;
        
        StageEditorObjectNew() = default;
        explicit StageEditorObjectNew(const Stage::StageObject& obj) 
            : EditorObjectBase<Stage::StageObject>(obj) {}
    };
    
    // ステージ用のコマンド（Modify改善版）
    struct StageEditorCommandNew : EditorCommandBase<StageEditorObjectNew>
    {
        // 基底クラスのoldData/newDataを使用
    };
    
    // ステージエディタマネージャ
    class StageEditorManager : public EditorManagerBase<StageEditorObjectNew, StageEditorCommandNew>
    {
    private:
        using Base = EditorManagerBase<StageEditorObjectNew, StageEditorCommandNew>;
        
    public:
        // ===== オブジェクト操作 =====
        
        size_t addObject(const Stage::StageObject& obj) override
        {
            StageEditorObjectNew editorObj(obj);
            editorObj.id = getNextId();
            
            m_objects.push_back(editorObj);
            
            if (!m_isExecutingCommand) {
                StageEditorCommandNew cmd;
                cmd.type = StageEditorCommandNew::Type::Add;
                cmd.object = editorObj;
                addToHistory(cmd);
            }
            
            return *editorObj.id;
        }
        
        void removeObject(size_t id) override
        {
            for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
                if (it->id == id) {
                    if (!m_isExecutingCommand) {
                        StageEditorCommandNew cmd;
                        cmd.type = StageEditorCommandNew::Type::Delete;
                        cmd.object = *it;
                        addToHistory(cmd);
                    }
                    
                    m_objects.erase(it);
                    m_selectedIds.erase(id);
                    break;
                }
            }
        }
        
        void moveObject(size_t id, const Vec2& newPos)
        {
            if (auto* obj = findObjectById(id)) {
                if (!m_isExecutingCommand) {
                    StageEditorCommandNew cmd;
                    cmd.type = StageEditorCommandNew::Type::Move;
                    cmd.object = *obj;
                    cmd.oldPos = obj->data.rect.pos;
                    cmd.newPos = newPos;
                    addToHistory(cmd);
                }
                
                obj->data.rect.setPos(newPos);
            }
        }
        
        void modifyObject(size_t id, const Stage::StageObject& newObj)
        {
            if (auto* obj = findObjectById(id)) {
                if (!m_isExecutingCommand) {
                    StageEditorCommandNew cmd;
                    cmd.type = StageEditorCommandNew::Type::Modify;
                    cmd.object = *obj;
                    cmd.oldData = obj->data;  // 改善：旧データを保存
                    cmd.newData = newObj;     // 改善：新データを保存
                    addToHistory(cmd);
                }
                
                obj->data = newObj;
            }
        }
        
        // ===== 選択オブジェクトの一括更新 =====
        
        void updateSelectedObjectsMovement(double distance, double speed, Stage::MovementType type)
        {
            for (auto& obj : m_objects) {
                if (obj.id && m_selectedIds.contains(*obj.id)) {
                    if (obj.data.type == Stage::StageType::MovingPlatform ||
                        obj.data.type == Stage::StageType::MovingDamagePlatform) {
                        obj.data.movementDistance = distance;
                        obj.data.movementSpeed = speed;
                        obj.data.movementType = type;
                    }
                }
            }
        }
        
        void updateSelectedObjectsDamage(double damage)
        {
            for (auto& obj : m_objects) {
                if (obj.id && m_selectedIds.contains(*obj.id)) {
                    if (obj.data.type == Stage::StageType::DamagePlatform ||
                        obj.data.type == Stage::StageType::MovingDamagePlatform) {
                        obj.data.damageAmount = damage;
                    }
                }
            }
        }
        
        // ===== 検索 =====
        
        Optional<size_t> findObjectAt(const Vec2& pos) const
        {
            // 逆順で検索（上に重なっているものを優先）
            for (auto it = m_objects.rbegin(); it != m_objects.rend(); ++it) {
                if (it->data.rect.contains(pos)) {
                    return it->id;
                }
            }
            return none;
        }
        
        bool hasObjectAtExactPosition(const RectF& rect) const
        {
            for (const auto& obj : m_objects) {
                if (obj.data.rect.x == rect.x &&
                    obj.data.rect.y == rect.y &&
                    obj.data.rect.w == rect.w &&
                    obj.data.rect.h == rect.h) {
                    return true;
                }
            }
            return false;
        }
        
        // 新しい矩形が既存のオブジェクトと重なっているかチェック
        bool hasOverlappingObject(const RectF& rect) const
        {
            for (const auto& obj : m_objects) {
                if (obj.data.rect.intersects(rect)) {
                    return true;
                }
            }
            return false;
        }
        
        // ===== ファイルI/O =====
        
        void saveToJSON(const FilePath& path) const override
        {
            Array<Stage::StageObject> mergedObjects = mergeAdjacentObjects();
            
            JSON json;
            Array<JSON> objectsArray;
            
            for (const auto& obj : mergedObjects) {
                objectsArray.push_back(stageObjectToJSON(obj));
            }
            
            json[U"objects"] = objectsArray;
            json.save(path);
        }
        
        void loadFromJSON(const FilePath& path) override
        {
            clear();
            
            const JSON json = JSON::Load(path);
            if (!json || !json.hasElement(U"objects")) return;
            
            m_isExecutingCommand = true;
            
            for (const auto& objJson : json[U"objects"].arrayView()) {
                if (auto obj = jsonToStageObject(objJson)) {
                    addObject(*obj);
                }
            }
            
            m_isExecutingCommand = false;
        }
        
    protected:
        // ===== コマンド実行（Modify改善版） =====
        
        void executeUndoCommand(const StageEditorCommandNew& cmd) override
        {
            switch (cmd.type) {
            case StageEditorCommandNew::Type::Add:
                if (cmd.object.id) {
                    removeObjectDirect(*cmd.object.id);
                }
                break;
                
            case StageEditorCommandNew::Type::Delete:
                m_objects.push_back(cmd.object);
                break;
                
            case StageEditorCommandNew::Type::Move:
                if (cmd.object.id && cmd.oldPos) {
                    if (auto* obj = findObjectById(*cmd.object.id)) {
                        obj->data.rect.setPos(*cmd.oldPos);
                    }
                }
                break;
                
            case StageEditorCommandNew::Type::Modify:
                // 改善：oldDataを使用して元に戻す
                if (cmd.object.id && cmd.oldData) {
                    if (auto* obj = findObjectById(*cmd.object.id)) {
                        obj->data = *cmd.oldData;
                    }
                }
                break;
            }
        }
        
        void executeRedoCommand(const StageEditorCommandNew& cmd) override
        {
            switch (cmd.type) {
            case StageEditorCommandNew::Type::Add:
                m_objects.push_back(cmd.object);
                break;
                
            case StageEditorCommandNew::Type::Delete:
                if (cmd.object.id) {
                    removeObjectDirect(*cmd.object.id);
                }
                break;
                
            case StageEditorCommandNew::Type::Move:
                if (cmd.object.id && cmd.newPos) {
                    if (auto* obj = findObjectById(*cmd.object.id)) {
                        obj->data.rect.setPos(*cmd.newPos);
                    }
                }
                break;
                
            case StageEditorCommandNew::Type::Modify:
                // 改善：newDataを使用してやり直す
                if (cmd.object.id && cmd.newData) {
                    if (auto* obj = findObjectById(*cmd.object.id)) {
                        obj->data = *cmd.newData;
                    }
                }
                break;
            }
        }
        
    private:
        // ===== ユーティリティ =====
        
        Array<Stage::StageObject> mergeAdjacentObjects() const
        {
            // 既存の実装をそのまま使用
            Array<Stage::StageObject> result;
            HashSet<size_t> merged;
            constexpr double epsilon = 0.5;
            
            for (size_t i = 0; i < m_objects.size(); ++i) {
                if (merged.contains(i)) continue;
                
                const auto& obj = m_objects[i].data;
                RectF mergedRect = obj.rect;
                merged.insert(i);
                
                bool foundMerge = true;
                while (foundMerge) {
                    foundMerge = false;
                    
                    for (size_t j = 0; j < m_objects.size(); ++j) {
                        if (merged.contains(j)) continue;
                        
                        const auto& other = m_objects[j].data;
                        
                        // 属性が一致しない場合はマージしない
                        if (obj.type != other.type ||
                            obj.groundSide != other.groundSide ||
                            obj.movementType != other.movementType ||
                            Math::Abs(obj.movementSpeed - other.movementSpeed) > epsilon ||
                            Math::Abs(obj.movementDistance - other.movementDistance) > epsilon ||
                            Math::Abs(obj.damageAmount - other.damageAmount) > epsilon) {
                            continue;
                        }
                        
                        const RectF otherRect = other.rect;
                        
                        // 横方向のマージ
                        if (Math::Abs(mergedRect.y - otherRect.y) < epsilon && 
                            Math::Abs(mergedRect.h - otherRect.h) < epsilon) {
                            if (Math::Abs((mergedRect.x + mergedRect.w) - otherRect.x) < epsilon) {
                                mergedRect.w = otherRect.x + otherRect.w - mergedRect.x;
                                merged.insert(j);
                                foundMerge = true;
                            }
                            else if (Math::Abs((otherRect.x + otherRect.w) - mergedRect.x) < epsilon) {
                                double rightEdge = mergedRect.x + mergedRect.w;
                                mergedRect.x = otherRect.x;
                                mergedRect.w = rightEdge - otherRect.x;
                                merged.insert(j);
                                foundMerge = true;
                            }
                        }
                        // 縦方向のマージ
                        else if (Math::Abs(mergedRect.x - otherRect.x) < epsilon && 
                                 Math::Abs(mergedRect.w - otherRect.w) < epsilon) {
                            if (Math::Abs((mergedRect.y + mergedRect.h) - otherRect.y) < epsilon) {
                                mergedRect.h = otherRect.y + otherRect.h - mergedRect.y;
                                merged.insert(j);
                                foundMerge = true;
                            }
                            else if (Math::Abs((otherRect.y + otherRect.h) - mergedRect.y) < epsilon) {
                                double bottomEdge = mergedRect.y + mergedRect.h;
                                mergedRect.y = otherRect.y;
                                mergedRect.h = bottomEdge - otherRect.y;
                                merged.insert(j);
                                foundMerge = true;
                            }
                        }
                    }
                }
                
                Stage::StageObject mergedObj = obj;
                mergedObj.rect = mergedRect;
                result.push_back(mergedObj);
            }
            
            return result;
        }
        
        JSON stageObjectToJSON(const Stage::StageObject& obj) const
        {
            JSON json;
            
            Array<int> rectArray = {
                static_cast<int>(obj.rect.x),
                static_cast<int>(obj.rect.y),
                static_cast<int>(obj.rect.w),
                static_cast<int>(obj.rect.h)
            };
            json[U"rect"] = rectArray;
            
            json[U"type"] = Stage::collisionTypeToString(obj.type);
            json[U"groundSide"] = Stage::groundSideToString(obj.groundSide);
            json[U"metadata"] = obj.metadata;
            
            // 移動設定
            if (obj.type == Stage::StageType::MovingPlatform ||
                obj.type == Stage::StageType::MovingDamagePlatform) {
                json[U"movementType"] = Stage::movementTypeToString(obj.movementType);
                json[U"movementSpeed"] = static_cast<int>(obj.movementSpeed);
                json[U"movementDistance"] = static_cast<int>(obj.movementDistance);
                json[U"loopMovement"] = obj.loopMovement;
            }
            
            // ダメージ設定
            if (obj.type == Stage::StageType::DamagePlatform ||
                obj.type == Stage::StageType::MovingDamagePlatform) {
                json[U"damageAmount"] = static_cast<int>(obj.damageAmount);
            }
            
            return json;
        }
        
        Optional<Stage::StageObject> jsonToStageObject(const JSON& json) const
        {
            Stage::StageObject obj;
            
            // rect
            if (!json.hasElement(U"rect") || !json[U"rect"].isArray()) {
                return none;
            }
            
            const auto& rectArray = json[U"rect"];
            if (rectArray.size() < 4) return none;
            
            obj.rect = RectF{
                rectArray[0].get<double>(),
                rectArray[1].get<double>(),
                rectArray[2].get<double>(),
                rectArray[3].get<double>()
            };
            
            // type, groundSide, metadata
            if (json.hasElement(U"type")) {
                obj.type = Stage::stringToCollisionType(json[U"type"].getString());
            }
            if (json.hasElement(U"groundSide")) {
                obj.groundSide = Stage::stringToGroundSide(json[U"groundSide"].getString());
            }
            if (json.hasElement(U"metadata")) {
                obj.metadata = json[U"metadata"].getString();
            }
            
            // movement
            if (json.hasElement(U"movementType")) {
                obj.movementType = Stage::stringToMovementType(json[U"movementType"].getString());
            }
            if (json.hasElement(U"movementSpeed")) {
                obj.movementSpeed = json[U"movementSpeed"].get<double>();
            }
            if (json.hasElement(U"movementDistance")) {
                obj.movementDistance = json[U"movementDistance"].get<double>();
            }
            if (json.hasElement(U"loopMovement")) {
                obj.loopMovement = json[U"loopMovement"].get<bool>();
            }
            
            // damage
            if (json.hasElement(U"damageAmount")) {
                obj.damageAmount = json[U"damageAmount"].get<double>();
            }
            
            return obj;
        }
    };
}