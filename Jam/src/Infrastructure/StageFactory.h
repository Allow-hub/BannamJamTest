#pragma once
#include "../Domain/Stage/IStage.h"
#include "../Domain/Stage/NormalStage.h"
#include "../Domain/Stage/MovingPlatformStage.h"
#include "../Domain/Physics/IPhysicsBody.h"
#include "StageLoader.h"
#include "IPhysicsBodyFactory.h"
#include "PhysicsFilterManager.h"

namespace Jam::Infrastructure {
    
    /**
     * ステージ生成結果
     * ステージと物理ボディを分離して管理
     */
    struct StageCreationResult {
        Array<std::unique_ptr<Domain::Stage::IStage>> stages;
        Array<std::shared_ptr<Domain::Physics::IPhysicsBody>> physicsBodies;
    };
    
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
         * @return 生成されたステージと物理ボディ
         */
        static StageCreationResult createStagesFromFile(
            const String& filename,
            std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory
        );
        
    private:
        /**
         * 個別のステージオブジェクトを生成
         * @param obj ステージオブジェクトデータ
         * @param bodyFactory 物理ボディファクトリー
         * @param outBody 生成された物理ボディ（出力）
         * @return 生成されたステージ
         */
        static std::unique_ptr<Domain::Stage::IStage> createStage(
            const Domain::Stage::StageObject& obj,
            std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory,
            std::shared_ptr<Domain::Physics::IPhysicsBody>& outBody
        );
        
        /**
         * StageTypeから適切な物理フィルターを取得
         */
        static Domain::Physics::PhysicsLayer getPhysicsLayerFromType(Domain::Stage::StageType type);
    };
}

