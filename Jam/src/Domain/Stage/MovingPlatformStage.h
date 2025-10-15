#pragma once
#include "IStage.h"
#include "StageTypes.h"
#include "../Physics/IPhysicsBody.h"
#include "../Physics/ICollisionListener.h"

namespace Jam::Domain::Stage {
    
    /**
     * 動的ステージの実装
     * 動く床・エレベーター・移動プラットフォームを管理
     */
    class MovingPlatformStage : public IStage, public Physics::ICollisionListener {
    private:
        struct MovingStageObject {
            StageObject visualData;
            std::shared_ptr<Physics::IPhysicsBody> physicsBody;
            Vec2 movementSpeed = {0, 0};
            Array<Vec2> movementPath;
            size_t currentPathIndex = 0;
            bool isDestroyed = false;
            bool isMoving = false;
            double pathTimer = 0.0;
        };
        
        Array<MovingStageObject> m_objects;
        HashSet<String> m_destroyedObjects;
        bool m_isLoaded = false;
        
    public:
        MovingPlatformStage() = default;
        explicit MovingPlatformStage(const Array<std::shared_ptr<Physics::IPhysicsBody>>& physicsBodies) {
            // TODO: 物理ボディ配列からMovingStageObjectを構築
            m_isLoaded = true;
        }
        
        // 動く床用のオブジェクト設定
        using PhysicsBodyFactory = std::function<std::shared_ptr<Physics::IPhysicsBody>(const RectF&, Physics::PhysicsLayer)>;
        
        void setObjects(const Array<StageObject>& objects, PhysicsBodyFactory bodyFactory) {
            m_objects.clear();
            m_destroyedObjects.clear();
            
            for (const auto& obj : objects) {
                MovingStageObject movingObj;
                movingObj.visualData = obj;
                movingObj.isDestroyed = false;
                
                // 当たり判定が必要なタイプのみ物理ボディを作成
                if (obj.type == StageType::Solid || 
                    obj.type == StageType::Platform ||
                    obj.type == StageType::Breakable) {
                    
                    // Kinematic Bodyとして作成（プログラム制御で動く）
                    auto physicsBody = bodyFactory(obj.rect, Physics::PhysicsLayer::Ground);
                    movingObj.physicsBody = physicsBody;
                } else {
                    movingObj.physicsBody = nullptr;
                }
                
                m_objects.push_back(movingObj);
            }
            
            m_isLoaded = true;
        }
        
        // IStage実装
        Array<StageObject> getRenderableObjects() const override {
            Array<StageObject> renderable;
            for (const auto& obj : m_objects) {
                if (!isObjectDestroyed(obj.visualData.metadata)) {
                    renderable.push_back(obj.visualData);
                }
            }
            return renderable;
        }
        
        Array<std::shared_ptr<Physics::IPhysicsBody>> getPhysicsBodies() const override {
            Array<std::shared_ptr<Physics::IPhysicsBody>> bodies;
            for (const auto& obj : m_objects) {
                if (obj.physicsBody && !isObjectDestroyed(obj.visualData.metadata)) {
                    bodies.push_back(obj.physicsBody);
                }
            }
            return bodies;
        }
        
        bool destroyObject(const String& objectId) override {
            for (auto& obj : m_objects) {
                if (obj.visualData.metadata == objectId && obj.visualData.destructible) {
                    m_destroyedObjects.insert(objectId);
                    obj.isDestroyed = true;
                    if (obj.physicsBody) {
                        obj.physicsBody->setLayer(Physics::PhysicsLayer::None);
                    }
                    return true;
                }
            }
            return false;
        }
        
        bool isObjectDestroyed(const String& objectId) const override {
            return m_destroyedObjects.contains(objectId);
        }
        
        // 動的機能実装
        void update(double deltaTime) override {
            for (auto& obj : m_objects) {
                if (!obj.isDestroyed && obj.isMoving && obj.physicsBody) {
                    updateMovement(obj, deltaTime);
                }
            }
        }
        
        void setMovementSpeed(const String& objectId, Vec2 speed) override {
            for (auto& obj : m_objects) {
                if (obj.visualData.metadata == objectId) {
                    obj.movementSpeed = speed;
                    obj.isMoving = (speed.length() > 0);
                    break;
                }
            }
        }
        
        void setMovementPath(const String& objectId, Array<Vec2> path) override {
            for (auto& obj : m_objects) {
                if (obj.visualData.metadata == objectId) {
                    obj.movementPath = path;
                    obj.currentPathIndex = 0;
                    obj.pathTimer = 0.0;
                    obj.isMoving = !path.empty();
                    break;
                }
            }
        }
        
        // ICollisionListener実装
        void onCollisionEnter(std::shared_ptr<Physics::IPhysicsBody> other) override {
            // Player が動く床に乗った時の処理
            if (other->getLayer() == Physics::PhysicsLayer::Player) {
                // TODO: Player追従開始の処理
            }
        }
        
        void onCollisionStay(std::shared_ptr<Physics::IPhysicsBody> other) override {
            // Player が動く床に乗り続けている時の処理
            if (other->getLayer() == Physics::PhysicsLayer::Player) {
                // Playerを床と一緒に移動させる
                for (const auto& obj : m_objects) {
                    if (obj.physicsBody && obj.isMoving && !obj.isDestroyed) {
                        // 床の移動量をPlayerに適用
                        Vec2 platformMovement = obj.movementSpeed * Scene::DeltaTime();
                        Vec2 currentPos = other->getPosition();
                        // other->setPosition(currentPos + platformMovement);
                    }
                }
            }
        }
        
        void onCollisionExit(std::shared_ptr<Physics::IPhysicsBody> other) override {
            // Player が動く床から離れた時の処理
            if (other->getLayer() == Physics::PhysicsLayer::Player) {
                // TODO: Player追従終了の処理
            }
        }
        
    private:
        void updateMovement(MovingStageObject& obj, double deltaTime) {
            if (obj.movementPath.empty()) {
                // 単純な速度ベース移動
                if (obj.physicsBody && obj.movementSpeed.length() > 0) {
                    Vec2 newPos = obj.physicsBody->getPosition() + obj.movementSpeed * deltaTime;
                    // obj.physicsBody->setPosition(newPos);
                    // 描画用データも更新
                    obj.visualData.rect.setCenter(newPos);
                }
            } else {
                // パスベース移動
                followPath(obj, deltaTime);
            }
        }
        
        void followPath(MovingStageObject& obj, double deltaTime) {
            if (obj.movementPath.size() < 2) return;
            
            Vec2 currentTarget = obj.movementPath[obj.currentPathIndex];
            Vec2 currentPos = obj.physicsBody->getPosition();
            
            // 目標地点までの距離
            Vec2 direction = currentTarget - currentPos;
            double distance = direction.length();
            
            if (distance < 5.0) { // 目標地点に到達
                obj.currentPathIndex = (obj.currentPathIndex + 1) % obj.movementPath.size();
                currentTarget = obj.movementPath[obj.currentPathIndex];
                direction = currentTarget - currentPos;
            }
            
            // 移動
            if (direction.length() > 0) {
                Vec2 moveDirection = direction.normalized();
                double moveSpeed = obj.movementSpeed.length() > 0 ? obj.movementSpeed.length() : 100.0;
                Vec2 newPos = currentPos + moveDirection * moveSpeed * deltaTime;
                
                // obj.physicsBody->setPosition(newPos);
                obj.visualData.rect.setCenter(newPos);
            }
        }
    };
}