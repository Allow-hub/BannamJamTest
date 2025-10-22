#pragma once
#include "../Domain/Stage/IStage.h"
#include "../Domain/Physics/IPhysicsBody.h"
#include "../Infrastructure/IPhysicsBodyFactory.h"
#include "../Infrastructure/StageFactory.h"

namespace Jam::UseCase {
    /**
     * ステージサービス
     * ステージの配列管理と更新を担当
     * 物理ボディも別途管理し、同期処理を実施
     */
    class StageService {
    private:
        Array<std::unique_ptr<Domain::Stage::IStage>> m_stages;
        Array<std::shared_ptr<Domain::Physics::IPhysicsBody>> m_physicsBodies;
        
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
            auto result = Infrastructure::StageFactory::createStagesFromFile(filename, bodyFactory);
            m_stages = std::move(result.stages);
            m_physicsBodies = std::move(result.physicsBodies);
            return !m_stages.isEmpty();
        }
        
        /**
         * 全ステージの更新
         */
        void update(double deltaTime) {
            for (auto& stage : m_stages) {
                stage->update(deltaTime);
            }
            
            syncPhysicsBodies();
        }
        
        /**
         * ステージの位置を物理ボディに同期
         */
        void syncPhysicsBodies() {
            for (size_t i = 0; i < m_stages.size(); ++i) {
                if (i < m_physicsBodies.size() && m_physicsBodies[i]) {
                    Vec2 currentCenter = m_stages[i]->getCurrentCenter();
                    m_physicsBodies[i]->setPos(currentCenter);
                }
            }
        }
        
        /**
         * ステージ配列の取得（読み取り専用）
         */
        const Array<std::unique_ptr<Domain::Stage::IStage>>& getStages() const {
            return m_stages;
        }
        
        /**
         * 物理ボディ配列の取得（読み取り専用）
         */
        const Array<std::shared_ptr<Domain::Physics::IPhysicsBody>>& getPhysicsBodies() const {
            return m_physicsBodies;
        }
        
        /**
         * ステージのクリア
         */
        void clear() {
            m_stages.clear();
            m_physicsBodies.clear();
        }
    };
}
