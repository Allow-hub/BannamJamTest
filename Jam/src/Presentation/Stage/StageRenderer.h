#pragma once
#include <Siv3D.hpp>
#include "../../Domain/Stage/StageTypes.h"
#include "../../Infrastructure/Siv3DPhysicsBody.h"

namespace Jam::Presentation::Stage {
    // 前方宣言
    class StageManager;

    class StageRenderer {
    public:
        StageRenderer() = default;
        ~StageRenderer() = default;
        
        // StageManagerとの関連付け
        void setStageManager(StageManager* stageManager) {
            m_stageManager = stageManager;
        }
        
        void draw();
        
		private:
        // 描画定数
        static constexpr int NORMAL_FRAME_THICKNESS = 2;
        static constexpr int MOVING_FRAME_THICKNESS = 3;
        
        // StageManagerへの参照
        StageManager* m_stageManager = nullptr;
    };

    // インライン実装（循環依存を避けるため最後に定義）
    inline void StageRenderer::draw() {
        if (!m_stageManager) return;
        
        // 通常ステージの描画
        const auto& normalObjects = m_stageManager->getNormalObjects();
        for (const auto& obj : normalObjects) {
            obj.rect.drawFrame(NORMAL_FRAME_THICKNESS, Palette::Blue);
        }
        
        // 動くプラットフォームの描画
        const auto& movingObjects = m_stageManager->getMovingObjects();
        const auto& movingBodies = m_stageManager->getMovingBodies();
        
        for (size_t i = 0; i < movingBodies.size() && i < movingObjects.size(); ++i) {
            Vec2 pos = movingBodies[i]->getPosition();
            SizeF size = movingObjects[i].rect.size;
            RectF(pos - size * 0.5, size).drawFrame(MOVING_FRAME_THICKNESS, Palette::Orange);
        }
    }
}
