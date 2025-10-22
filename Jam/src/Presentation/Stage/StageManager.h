#pragma once
#include "../../UseCase/StageService.h"
#include "../../Domain/Stage/StageTypes.h"
#include "../../Infrastructure/TextureLoader.h"

namespace Jam::Presentation::Stage {
    /**
     * ステージマネージャー
     * ステージの描画管理を担当（updateなし）
     */
    class StageManager {
    private:
        std::shared_ptr<UseCase::StageService> m_stageService;
        
    public:
        /**
         * サービスの設定
         */
        void setService(std::shared_ptr<UseCase::StageService> service) {
            m_stageService = service;
        }
        
        /**
         * テクスチャの読み込み
         */
        void loadTextures() {
            Infrastructure::TextureLoader::preloadStageTextures();
        }
        
        /**
         * 描画のみ
         */
        void draw() const {
            if (!m_stageService) return;
            
            for (const auto& stage : m_stageService->getStages()) {
                drawStage(stage.get());
            }
        }
        
    private:
        /**
         * 個別のステージを描画
         */
        void drawStage(const Domain::Stage::IStage* stage) const {
            if (!stage) return;
            
            const RectF rect = stage->getRenderRect();
            const auto type = stage->getType();
            
            Texture texture = Infrastructure::TextureLoader::getStageTexture(type);
            if (texture) {
                texture.resized(rect.size).draw(rect.pos);
            }
        }
    };
}
