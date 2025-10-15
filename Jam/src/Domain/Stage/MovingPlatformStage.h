#pragma once
#include "IStage.h"
#include "StageTypes.h"
#include "../Physics/IPhysicsBody.h"

namespace Jam::Domain::Stage {
    
    /**
     * 動的ステージの実装
     */
    class MovingPlatformStage : public IStage {
    private:
        Vec2 m_movementSpeed = {50.0, 0.0}; // デフォルトの移動速度
        bool m_isLoaded = false;
        
    public:
        MovingPlatformStage() = default;
        explicit MovingPlatformStage(std::shared_ptr<Physics::IPhysicsBody> physicsBody) 
            : m_isLoaded(true)
        {
            m_body = physicsBody;
        }
        
        // IStage実装
        Array<StageObject> getRenderableObjects() const override {
            Array<StageObject> renderable;
            // m_bodyから位置を取得して描画用オブジェクトを返す
            if (m_body) {
                StageObject obj;
                obj.rect = RectF(Arg::center = m_body->getPosition(), 100, 20); // 100x20のプラットフォーム
                obj.type = StageType::Platform;
                obj.metadata = U"moving_platform";
                renderable.push_back(obj);
            }
            return renderable;
        }
        
        Array<std::shared_ptr<Physics::IPhysicsBody>> getPhysicsBodies() const override {
            Array<std::shared_ptr<Physics::IPhysicsBody>> bodies;
            if (m_body) {
                bodies.push_back(m_body);
            }
            return bodies;
        }
        
        bool destroyObject(const String& objectId) override {
            return false; // 動くプラットフォームは破壊不可
        }
        
        bool isObjectDestroyed(const String& objectId) const override {
            return false;
        }
        
        bool isLoaded() const {
            return m_isLoaded;
        }
        
        // 動的機能実装
        void update(double deltaTime) override {
            if (m_body) {
                // setVelocityを使って実際に動かす
                m_body->setVelocity(m_movementSpeed);
                
                // 画面端で反転
                Vec2 pos = m_body->getPosition();
                if (pos.x > 800 || pos.x < 200) {
                    m_movementSpeed.x *= -1;
                }
            }
        }
        
        void setMovementSpeed(const String& objectId, Vec2 speed) override {
            m_movementSpeed = speed;
        }
        
        void setMovementPath(const String& objectId, Array<Vec2> path) override {
            // 簡略化のため実装なし
        }
    };
}