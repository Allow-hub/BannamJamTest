#include "stdafx.h"
#include "UseCase/BlockService.h"
#include "Domain/Events/GameEvents.h"
#include "Infrastructure/BlockFactory.h"
#include "Infrastructure/Siv3DPhysicsBody.h"

namespace Jam::UseCase {

    bool BlockService::initialize(
        const String& filename,
        std::shared_ptr<Infrastructure::Locator::IPhysicsBodyFactory> bodyFactory,
        Domain::Events::GameEventQueue& eventQueue,
        Domain::Physics::PhysicsBodyID playerId
    ) {
        auto result = Infrastructure::BlockFactory::createBlocksFromFile(filename, bodyFactory, eventQueue, playerId);
        m_stages = std::move(result.stages);
        m_physicsBodies = std::move(result.physicsBodies);
        m_bodyIndices = std::move(result.bodyIndices);
        m_bodyOffsets = std::move(result.bodyOffsets);
        m_bodyGroundSides = std::move(result.bodyGroundSides);
        
        return !m_stages.isEmpty();
    }

    void BlockService::update(double deltaTime) {
        for (auto& stage : m_stages) {
            stage->update(deltaTime);
        }
        
        syncPhysicsBodies();
    }

    void BlockService::syncPhysicsBodies() {
        for (size_t i = 0; i < m_stages.size(); ++i) {
            if (i >= m_bodyIndices.size()) continue;
            
            Vec2 currentCenter = m_stages[i]->getCurrentCenter();
            
            // このステージに属する全ての物理ボディを同期
            for (size_t bodyIdx : m_bodyIndices[i]) {
                if (bodyIdx < m_physicsBodies.size() && m_physicsBodies[bodyIdx]) {
                    // オフセットを考慮した位置に設定
                    Vec2 bodyPosition = currentCenter + m_bodyOffsets[bodyIdx];
                    m_physicsBodies[bodyIdx]->setPos(bodyPosition);
                }
            }
        }
    }

    const Array<std::unique_ptr<Domain::Block::IBlock>>& BlockService::getStages() const {
        return m_stages;
    }

    const Array<std::shared_ptr<Domain::Physics::IPhysicsBody>>& BlockService::getPhysicsBodies() const {
        return m_physicsBodies;
    }

    void BlockService::clear() {
        m_stages.clear();
        m_physicsBodies.clear();
        m_bodyIndices.clear();
        m_bodyOffsets.clear();
    }

    bool BlockService::checkOneWayPlatformLanding(
        std::shared_ptr<Domain::Physics::IPhysicsBody> playerBody,
        bool isPressingDown,
        double playerHeight
    ) {
        if (!playerBody) return false;
        
        // 下ボタンが押されている場合は着地判定をスキップ
        if (isPressingDown) return false;
        
        auto playerPos = playerBody->getPosition();
        auto playerVel = playerBody->getVelocity();
        
        if (playerVel.y <= 0) return false;
        
        constexpr double landingThreshold = 5.0;
        
        for (const auto& stage : m_stages) {
            if (stage->getType() != Domain::Block::BlockType::OneWayPlatform) continue;
            
            auto platformRect = stage->getRenderRect();
            double platformTop = platformRect.y;
            double playerBottom = playerPos.y + playerHeight / 2.0;
            
            if (playerPos.x < platformRect.x || playerPos.x > platformRect.x + platformRect.w) continue;
            
            if (playerBottom < platformTop - landingThreshold || playerBottom > platformTop + landingThreshold) continue;
            
            playerBody->setPos({ playerPos.x, platformTop - playerHeight / 2.0 });
            playerBody->setVelocity({ playerVel.x, 0.0 });
            return true;
        }
        
        return false;
    }

    void BlockService::drawPhysicsLayerDebug() const {
        for (size_t bodyIdx = 0; bodyIdx < m_physicsBodies.size(); ++bodyIdx) {
            const auto& body = m_physicsBodies[bodyIdx];
            if (!body) continue;
            
            // Siv3DPhysicsBodyにキャスト
            auto siv3dBody = std::dynamic_pointer_cast<Jam::Infrastructure::Physics::Siv3DPhysicsBody>(body);
            if (!siv3dBody) continue;
            
            const auto layer = body->getLayer();
            const P2Body& p2body = siv3dBody->getBody();
            
            // レイヤーごとに色分け
            ColorF color;
            bool shouldDraw = false;
            
            switch (layer) {
            case Jam::Domain::Physics::PhysicsLayer::Ground:
            {
                // GroundSide情報を使って色分け
                if (bodyIdx < m_bodyGroundSides.size()) {
                    const auto groundSide = m_bodyGroundSides[bodyIdx];
                    
                    // 上面・下面ともに緑色で描画
                    if (groundSide == Domain::Block::GroundSide::Up || groundSide == Domain::Block::GroundSide::Down) {
                        color = ColorF(0.0, 1.0, 0.0, 0.1);  // 緑 = チョーカー可能（上面・下面）
                        shouldDraw = true;
                    }
                }
                break;
            }
            case Jam::Domain::Physics::PhysicsLayer::Wall:
                // color = ColorF(1.0, 0.0, 0.0, 0.5);  // 赤 = Wall
                break;
            case Jam::Domain::Physics::PhysicsLayer::OneWayPlatform:
                // color = ColorF(0.0, 0.5, 1.0, 0.5);  // 青 = すり抜け床
                break;
            default:
                // color = ColorF(0.5, 0.5, 0.5, 0.5);  // 灰 = その他
                break;
            }
            
            // 描画が必要な場合のみP2Bodyの図形を描画
            if (shouldDraw) {
                //p2body.draw(color);
                p2body.drawFrame(2, ColorF(color, 1.0));
            }
        }
    }

}