#pragma once
#include <Siv3D.hpp>
#include "../../Domain/Stage/StageTypes.h"
#include "../../Infrastructure/StageLoader.h"
#include "../../Infrastructure/Siv3DPhysicsBody.h"

namespace Jam::Presentation::Stage {
	class StageManager {
	public:
		StageManager() = default;
		~StageManager() = default;

		void initialize(P2World& world, std::vector<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>>& physicsBodies) {
			m_world = &world;
			m_physicsBodies = &physicsBodies;
			loadAndCreateStages();
		}

		void update() {
			updateMovingPlatforms();
		}

		// データアクセスメソッド（StageRendererで使用）
		const Array<Jam::Domain::Stage::StageObject>& getNormalObjects() const { return m_normalObjects; }
		const Array<Jam::Domain::Stage::StageObject>& getMovingObjects() const { return m_movingObjects; }
		const Array<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>>& getMovingBodies() const { return m_movingBodies; }

	private:
		// 定数
		static constexpr double SCREEN_MARGIN = 50.0;

		// 物理世界への参照
		P2World* m_world = nullptr;
		std::vector<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>>* m_physicsBodies = nullptr;

		// ステージデータ
		Array<Jam::Domain::Stage::StageObject> m_normalObjects;
		Array<Jam::Domain::Stage::StageObject> m_movingObjects;
		Array<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>> m_movingBodies;
		Array<Vec2> m_movingVelocities;

		void loadAndCreateStages() {
			// JSONからステージデータを読み込み
			Jam::Infrastructure::Stage::StageLoader loader;
			if (!loader.loadSeparatedStagesFromFile(U"stage1.json", m_normalObjects, m_movingObjects)) {
				Print << U"[StageManager] ❌ Failed to load stage1.json";
				return;
			}

			Print << U"[StageManager] ✅ Loaded " << m_normalObjects.size()
				<< U" normal and " << m_movingObjects.size() << U" moving stages";

			// 物理ボディを作成
			createPhysicsBodies();
		}

		void createPhysicsBodies() {
			if (!m_world || !m_physicsBodies) return;

			// 通常ステージの作成
			for (const auto& obj : m_normalObjects) {
				auto body = createPhysicsBody(obj, P2BodyType::Static);
				m_physicsBodies->push_back(body);
			}

			// 動くプラットフォームの作成
			for (const auto& obj : m_movingObjects) {
				auto body = createPhysicsBody(obj, P2BodyType::Kinematic);
				m_movingBodies.push_back(body);
				m_movingVelocities.push_back(obj.movementSpeed);
				m_physicsBodies->push_back(body);
			}
		}

		std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody> createPhysicsBody(
			const Jam::Domain::Stage::StageObject& obj, P2BodyType bodyType) {
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

		void updateMovingPlatforms() {
			for (size_t i = 0; i < m_movingBodies.size(); ++i) {
				Vec2 currentPos = m_movingBodies[i]->getPosition();
				Vec2 newPos = currentPos + m_movingVelocities[i] * Scene::DeltaTime();

				// 画面端での反転
				if (newPos.x <= SCREEN_MARGIN || newPos.x >= Scene::Width() - SCREEN_MARGIN) {
					m_movingVelocities[i].x *= -1;
				}
				if (newPos.y <= SCREEN_MARGIN || newPos.y >= Scene::Height() - SCREEN_MARGIN) {
					m_movingVelocities[i].y *= -1;
				}

				m_movingBodies[i]->setPos(newPos);
			}
		}
	};
}
