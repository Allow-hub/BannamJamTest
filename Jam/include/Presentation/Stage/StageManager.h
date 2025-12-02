#pragma once
#include <memory>
#include "UseCase/StageService.h"
#include "Domain/Stage/IStage.h"

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
        void setService(std::shared_ptr<UseCase::StageService> service);
        
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
        void drawStage(const Domain::Stage::IStage* stage) const;
    };
}
