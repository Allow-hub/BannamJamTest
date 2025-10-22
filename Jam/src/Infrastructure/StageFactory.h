#pragma once
#include "../Domain/Stage/IStage.h"
#include "../Domain/Stage/NormalStage.h"
#include "../Domain/Stage/MovingPlatformStage.h"
#include "StageLoader.h"
#include "IPhysicsBodyFactory.h"
#include "PhysicsFilterManager.h"

namespace Jam::Infrastructure {
    /**
     * ステージファクトリー
     * JSONファイルからステージオブジェクトの配列を生成
     */
    class StageFactory {
    public:
        /**
         * JSONファイルから全ステージを生成
         * @param filename ステージファイル名
         * @param bodyFactory 物理ボディファクトリー
         * @return 生成されたステージの配列
         */
        static Array<std::unique_ptr<Domain::Stage::IStage>> createStagesFromFile(
            const String& filename,
            std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory
        );
        
    private:
        /**
         * 個別のステージオブジェクトを生成
         * @param obj ステージオブジェクトデータ
         * @param bodyFactory 物理ボディファクトリー
         * @return 生成されたステージ
         */
        static std::unique_ptr<Domain::Stage::IStage> createStage(
            const Domain::Stage::StageObject& obj,
            std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory
        );
        
        /**
         * StageTypeから適切な物理フィルターを取得
         */
        static Domain::Physics::PhysicsLayer getPhysicsLayerFromType(Domain::Stage::StageType type);
    };
}

