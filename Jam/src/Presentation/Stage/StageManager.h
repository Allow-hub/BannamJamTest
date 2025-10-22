#pragma once
#include <Siv3D.hpp>
#include "../../UseCase/StageService.h"

namespace Jam::Presentation::Stage {
	/**
	 * StageManager - Presentation層のシンプルなラッパー
	 * UseCase層のStageServiceを呼び出すだけの軽量クラス
	 */
	class StageManager {
	public:
		StageManager() = default;
		~StageManager() = default;

		bool initialize(P2World& world, 
		               std::vector<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>>& physicsBodies) {
			m_stageService = std::make_unique<Jam::UseCase::StageService>();
			return m_stageService->initializeStage(U"stage1.json", world, physicsBodies);
		}

		void update(double deltaTime) {
			if (m_stageService) {
				m_stageService->update(deltaTime);
			}
		}

		// データアクセス（StageServiceに完全委譲）
		const Array<Jam::Domain::Stage::StageObject>& getNormalObjects() const { 
			return m_stageService ? m_stageService->getNormalObjects() : m_emptyObjects; 
		}
		
		const Array<Jam::Domain::Stage::StageObject>& getMovingObjects() const { 
			return m_stageService ? m_stageService->getMovingObjects() : m_emptyObjects; 
		}
		
		const Array<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>>& getMovingBodies() const { 
			return m_stageService ? m_stageService->getMovingBodies() : m_emptyBodies; 
		}

	private:
		std::unique_ptr<Jam::UseCase::StageService> m_stageService;
		
		// 空のコンテナ（フォールバック用）
		static inline Array<Jam::Domain::Stage::StageObject> m_emptyObjects;
		static inline Array<std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>> m_emptyBodies;
	};
}
