#pragma once
#include <memory>
#include "UseCase/BlockService.h"
#include "Domain/Block/IBlock.h"

namespace Jam::Presentation::Block {
    /**
     * ステージマネージャー
     * ステージの描画管理を担当（updateなし）
     */
    class BlockManager {
    private:
        std::shared_ptr<UseCase::BlockService> m_stageService;
        
    public:
        /**
         * サービスの設定
         */
        void setService(std::shared_ptr<UseCase::BlockService> service);
        
        /**
         * テクスチャの読み込み
         */
        void loadTextures();
        
        /**
         * 描画のみ
         */
        void draw() const;
        
    private:
        /**
         * 個別のステージを描画
         */
        void drawStage(const Domain::Block::IBlock* stage) const;
    };
}
