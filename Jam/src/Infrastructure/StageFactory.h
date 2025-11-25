#pragma once
#include "../Domain/Stage/IStage.h"
#include "../Domain/Stage/NormalStage.h"
#include "../Domain/Stage/MovingPlatformStage.h"
#include "../Domain/Stage/OneWayPlatformStage.h"
#include "../Domain/Stage/DamageStage.h"
#include "../Domain/Stage/MovingDamagePlatformStage.h"
#include "../Domain/Physics/IPhysicsBody.h"
#include "../Domain/Events/GameEvents.h"
#include "StageLoader.h"
#include "IPhysicsBodyFactory.h"
#include "PhysicsFilterManager.h"

namespace Jam::Infrastructure {
    
    /**
     * ステージ生成結果
     * ステージと物理ボディを紐づけて返却
     */
    struct StageCreationResult {
        Array<std::unique_ptr<Domain::Stage::IStage>> stages;
        Array<std::shared_ptr<Domain::Physics::IPhysicsBody>> physicsBodies;
        // 各ステージに対応する物理ボディのインデックス配列
        // stages[i]はphysicsBodies[bodyIndices[i][0], bodyIndices[i][1], ...]を参照
        Array<Array<size_t>> bodyIndices;
        // 各物理ボディの基準位置からのオフセット
        Array<Vec2> bodyOffsets;
        // 各物理ボディのGroundSide情報(デバッグ描画用)
        Array<Domain::Stage::GroundSide> bodyGroundSides;
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
         * @param eventQueue イベントキュー(ダメージ床用)
         * @param playerId プレイヤーID(ダメージ床用)
         * @return 生成されたステージと物理ボディ
         */
        static StageCreationResult createStagesFromFile(
            const String& filename,
            std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory,
            Domain::Events::GameEventQueue& eventQueue,
            Domain::Physics::PhysicsBodyID playerId
        );
        
    private:
        /**
         * 個別のステージオブジェクトを生成
         * @param obj ステージオブジェクトデータ
         * @param bodyFactory 物理ボディファクトリー
         * @param outBody 生成された物理ボディ(出力)
         * @param eventQueue イベントキュー(ダメージ床用)
         * @param playerId プレイヤーID(ダメージ床用)
         * @return 生成されたステージ
         */
        static std::unique_ptr<Domain::Stage::IStage> createStage(
            const Domain::Stage::StageObject& obj,
            std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory,
            std::shared_ptr<Domain::Physics::IPhysicsBody>& outBody,
            Domain::Events::GameEventQueue& eventQueue,
            Domain::Physics::PhysicsBodyID playerId
        );
        
        /**
         * StageTypeとGroundSideから適切な物理レイヤーを取得
         */
        static Domain::Physics::PhysicsLayer getPhysicsLayerFromType(
            Domain::Stage::StageType type,
            Domain::Stage::GroundSide groundSide = Domain::Stage::GroundSide::Up
        );
        
        /**
         * groundSideに基づいてオブジェクトを分割
         * 1つのオブジェクトから壁・床・複数オブジェクトを生成
         */
        static Array<Domain::Stage::StageObject> expandObjectByGroundSide(const Domain::Stage::StageObject& obj);
    };
}

