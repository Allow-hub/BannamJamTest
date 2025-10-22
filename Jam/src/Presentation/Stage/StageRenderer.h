#pragma once
#include <Siv3D.hpp>
#include "../../Domain/Stage/StageTypes.h"
#include "../../Infrastructure/Siv3DPhysicsBody.h"
#include "../../Infrastructure/TextureLoader.h"

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
        // === 描画定数 ===
        static constexpr int NORMAL_STAGE_FRAME_THICKNESS = 2;
        static constexpr int MOVING_STAGE_FRAME_THICKNESS = 3;
        static constexpr double CENTER_OFFSET_FACTOR = 0.5;
        
        // StageManagerへの参照
        StageManager* m_stageManager = nullptr;
    };

    // インライン実装
    inline void StageRenderer::draw() {
        if (!m_stageManager) return;
        
        // 通常ステージの描画（テクスチャ使用）
        const auto& normalObjects = m_stageManager->getNormalObjects();
        for (const auto& obj : normalObjects) {
            Texture texture = Jam::Infrastructure::TextureLoader::getStageTexture(Jam::Domain::Stage::StageType::Normal);
            if (texture) {
                texture.resized(obj.rect.size).draw(obj.rect.pos);
            } else {
                // フォールバック: フレーム描画
                obj.rect.drawFrame(NORMAL_STAGE_FRAME_THICKNESS, Palette::Blue);
            }
        }
        
        // 動くプラットフォームの描画（テクスチャ使用）
        const auto& movingObjects = m_stageManager->getMovingObjects();
        const auto& movingBodies = m_stageManager->getMovingBodies();
        
        for (size_t i = 0; i < movingBodies.size() && i < movingObjects.size(); ++i) {
            Vec2 pos = movingBodies[i]->getPosition();
            SizeF size = movingObjects[i].rect.size;
            RectF drawRect(pos - size * CENTER_OFFSET_FACTOR, size);
            
            Texture texture = Jam::Infrastructure::TextureLoader::getStageTexture(Jam::Domain::Stage::StageType::Platform);
            if (texture) {
                texture.resized(drawRect.size).draw(drawRect.pos);
            } else {
                // フォールバック: フレーム描画
                drawRect.drawFrame(MOVING_STAGE_FRAME_THICKNESS, Palette::Orange);
            }
        }
    }
}
