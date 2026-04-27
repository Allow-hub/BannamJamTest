#include "Presentation/Block/BlockManager.h"
#include "Domain/Block/BlockTypes.h"
#include "Domain/Block/NormalBlock.h"
#include "Infrastructure/TextureLoader.h"
#include "Presentation/TextureManager.h"

namespace Jam::Presentation::Block {

    void BlockManager::setService(std::shared_ptr<UseCase::BlockService> service) {
        m_stageService = service;
    }

    void BlockManager::loadTextures() {
        Infrastructure::TextureLoader::preloadBlockTextures();
    }

    void BlockManager::draw() const {
        if (!m_stageService) return;
        
        for (const auto& stage : m_stageService->getStages()) {
            drawStage(stage.get());
        }
    }

    void BlockManager::drawStage(const Domain::Block::IBlock* stage) const {
        if (!stage) return;

        const RectF rect = stage->getRenderRect();
        const auto type = stage->getType();

        const Vec2 roundedPos = rect.pos.asPoint();
        const Size roundedSize = rect.size.asPoint();

        Texture texture;
        
        // NormalStageの場合、texturePathを取得して使用
        if (type == Domain::Block::BlockType::Normal) {
            const auto* normalStage = dynamic_cast<const Domain::Block::NormalBlock*>(stage);
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
    }

}