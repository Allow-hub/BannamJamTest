#pragma once
#include <Siv3D.hpp>
#include "../Domain/Stage/StageTypes.h"
#include "../Infrastructure/StageLoader.h"
#include "../Infrastructure/Siv3DPhysicsBody.h"

namespace Jam::UseCase {
    /**
     * StageService - ステージのビジネスロジック専用
     * 必要最小限の機能のみ提供
     */
    class StageService {
    public:
        StageService() = default;
        ~StageService() = default;

        // === 初期化 ===
        bool initializeStage(const String& stageFileName, 
                           P2World& world, 
                           std::vector<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>>& physicsBodies);

        // === Update処理（メインのビジネスロジック） ===
        void update(double deltaTime);

        // === データアクセス（Presentation層用） ===
        const Array<Jam::Domain::Stage::StageObject>& getNormalObjects() const { return m_normalObjects; }
        const Array<Jam::Domain::Stage::StageObject>& getMovingObjects() const { return m_movingObjects; }
        const Array<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>>& getMovingBodies() const { return m_movingBodies; }

        // === ステージ状態 ===
        bool isStageLoaded() const { return m_isLoaded; }

    private:
        // === ビジネスルール定数 ===
        static constexpr double SCREEN_MARGIN = 50.0;

        // === データ ===
        bool m_isLoaded = false;
        Array<Jam::Domain::Stage::StageObject> m_normalObjects;
        Array<Jam::Domain::Stage::StageObject> m_movingObjects;
        Array<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>> m_movingBodies;
        Array<Vec2> m_movingVelocities;
        
        // === Infrastructure層への参照 ===
        P2World* m_world = nullptr;
        std::vector<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>>* m_physicsBodies = nullptr;

        // === プライベートメソッド ===
        void updateMovingPlatforms(double deltaTime);
        void createPhysicsBodies();
        std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody> createPhysicsBody(
            const Jam::Domain::Stage::StageObject& obj, 
            P2BodyType bodyType) const;
    };

    // === インライン実装 ===
    inline bool StageService::initializeStage(const String& stageFileName, 
                                             P2World& world, 
                                             std::vector<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>>& physicsBodies) {
        m_world = &world;
        m_physicsBodies = &physicsBodies;
        
        // Infrastructure層を使用してデータロード
        Jam::Infrastructure::Stage::StageLoader loader;
        if (!loader.loadSeparatedStagesFromFile(stageFileName, m_normalObjects, m_movingObjects)) {
            Print << U"[StageService] ❌ Failed to load " << stageFileName;
            m_isLoaded = false;
            return false;
        }

        Print << U"[StageService] ✅ Loaded " << m_normalObjects.size()
              << U" normal and " << m_movingObjects.size() << U" moving stages";

        createPhysicsBodies();
        m_isLoaded = true;
        return true;
    }

    inline void StageService::update(double deltaTime) {
        if (!m_isLoaded) return;
        updateMovingPlatforms(deltaTime);
    }

    inline void StageService::updateMovingPlatforms(double deltaTime) {
        for (size_t i = 0; i < m_movingBodies.size() && i < m_movingVelocities.size(); ++i) {
            if (i >= m_movingBodies.size() || !m_movingBodies[i]) continue;

            Vec2 currentPos = m_movingBodies[i]->getPosition();
            Vec2 newPos = currentPos + m_movingVelocities[i] * deltaTime;

            // ビジネスルール：画面端での反転
            if (newPos.x <= SCREEN_MARGIN || newPos.x >= Scene::Width() - SCREEN_MARGIN) {
                m_movingVelocities[i].x *= -1;
            }
            if (newPos.y <= SCREEN_MARGIN || newPos.y >= Scene::Height() - SCREEN_MARGIN) {
                m_movingVelocities[i].y *= -1;
            }

            m_movingBodies[i]->setPos(newPos);
        }
    }

    inline void StageService::createPhysicsBodies() {
        if (!m_world || !m_physicsBodies) return;

        // 通常ステージの物理ボディ作成
        for (const auto& obj : m_normalObjects) {
            auto body = createPhysicsBody(obj, P2BodyType::Static);
            m_physicsBodies->push_back(body);
        }

        // 動くプラットフォームの物理ボディ作成
        for (const auto& obj : m_movingObjects) {
            auto body = createPhysicsBody(obj, P2BodyType::Kinematic);
            m_movingBodies.push_back(body);
            m_movingVelocities.push_back(obj.movementSpeed);
            m_physicsBodies->push_back(body);
        }
    }

    inline std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody> 
    StageService::createPhysicsBody(const Jam::Domain::Stage::StageObject& obj, P2BodyType bodyType) const {
        auto body = std::make_shared<Jam::Infrastructure::Physics::Siv3DPhysicsBody>(
            *m_world,
            obj.rect.center(),
            obj.rect.size,
            bodyType,
            Jam::Domain::Physics::PhysicsMaterial{ 1.0, 0.0, 0.0 }
        );
        body->setLayer(Jam::Domain::Physics::PhysicsLayer::Ground);
        return body;
    }
}
