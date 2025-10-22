#pragma once
#include "../Domain/Stage/IStage.h"
#include "../Infrastructure/IPhysicsBodyFactory.h"
#include "../Infrastructure/StageFactory.h"

namespace Jam::UseCase {
    /**
     * ステージサービス
     * ステージの配列管理と更新を担当
     */
    class StageService {
    private:
        Array<std::unique_ptr<Domain::Stage::IStage>> m_stages;
        
    public:
        /**
         * ステージの初期化
         * @param filename ステージファイル名
         * @param bodyFactory 物理ボディファクトリー
         * @return 初期化成功したか
         */
        bool initialize(
            const String& filename,
            std::shared_ptr<Infrastructure::Locator::IPhysicsBodyFactory> bodyFactory
        ) {
            m_stages = Infrastructure::StageFactory::createStagesFromFile(filename, bodyFactory);
            return !m_stages.isEmpty();
        }
        
        /**
         * 全ステージの更新
         */
        void update(double deltaTime) {
            for (auto& stage : m_stages) {
                stage->update(deltaTime);
            }
        }
        
        /**
         * ステージ配列の取得（読み取り専用）
         */
        const Array<std::unique_ptr<Domain::Stage::IStage>>& getStages() const {
            return m_stages;
        }
        
        /**
         * ステージのクリア
         */
        void clear() {
            m_stages.clear();
        }
    };
}
