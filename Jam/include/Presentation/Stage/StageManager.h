#pragma once
#include "UseCase/StageService.h"
#include "Domain/Stage/StageTypes.h"
#include "Domain/Stage/NormalStage.h"
#include "Infrastructure/TextureLoader.h"
#include "Presentation/TextureManager.h"

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

			const Vec2 roundedPos = rect.pos.asPoint();
			const Size roundedSize = rect.size.asPoint();

			Texture texture;
			
			// NormalStageの場合、texturePathを取得して使用
			if (type == Domain::Stage::StageType::Normal) {
				const auto* normalStage = dynamic_cast<const Domain::Stage::NormalStage*>(stage);
				if (normalStage && !normalStage->getTexturePath().isEmpty()) {
					texture = TextureManager::Load(normalStage->getTexturePath());
				}
			}
			
			if (!texture) {
				texture = Infrastructure::TextureLoader::getStageTexture(type);
			}
			
			if (!texture) return;

			//  テクスチャアドレスモードをスコープ内だけ Repeat にする
			const ScopedRenderStates2D sampler{ SamplerState::RepeatLinear };

			//  ステージ範囲に合わせてタイル状に敷き詰め
			texture.mapped(roundedSize).draw(roundedPos);

			//  スコープを抜けると自動で Clamp に戻る
		}

    };
}
