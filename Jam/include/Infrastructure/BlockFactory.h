#pragma once
#include <memory>
#include <Siv3D.hpp>
#include "Domain/Block/IBlock.h"
#include "Domain/Block/BlockTypes.h"
#include "Domain/Physics/IPhysicsBody.h"
#include "Domain/Physics/PhysicsBodyID.h"
#include "Infrastructure/IPhysicsBodyFactory.h"
#include "Domain/Events/GameEvents.h"

namespace Jam::Infrastructure {
    
    /**
     * ステージ生成結果
     * ステージと物理ボディを紐づけて返却
     */
    struct BlockCreationResult {
        Array<std::unique_ptr<Domain::Block::IBlock>> stages;
        Array<std::shared_ptr<Domain::Physics::IPhysicsBody>> physicsBodies;
        // 各ステージに対応する物理ボディのインデックス配列
        // stages[i]はphysicsBodies[bodyIndices[i][0], bodyIndices[i][1], ...]を参照
        Array<Array<size_t>> bodyIndices;
        // 各物理ボディの基準位置からのオフセット
        Array<Vec2> bodyOffsets;
        // 各物理ボディのGroundSide情報(デバッグ描画用)
        Array<Domain::Block::GroundSide> bodyGroundSides;
    };
    
    /**
     * ステージファクトリー
     * JSONファイルからステージオブジェクトの配列を生成
     */
    class BlockFactory {
    public:
        /**
         * JSONファイルから全ステージを生成
         * @param filename ステージファイル名
         * @param bodyFactory 物理ボディファクトリー
         * @param eventQueue イベントキュー(ダメージ床用)
         * @param playerId プレイヤーID(ダメージ床用)
         * @return 生成されたステージと物理ボディ
         */
        static BlockCreationResult createBlocksFromFile(
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
        static std::unique_ptr<Domain::Block::IBlock> createBlock(
            const Domain::Block::BlockObject& obj,
            std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory,
            std::shared_ptr<Domain::Physics::IPhysicsBody>& outBody,
            Domain::Events::GameEventQueue& eventQueue,
            Domain::Physics::PhysicsBodyID playerId
        );
        
        /**
         * StageTypeとGroundSideから適切な物理レイヤーを取得
         */
        static Domain::Physics::PhysicsLayer getPhysicsLayerFromType(
            Domain::Block::BlockType type,
            Domain::Block::GroundSide groundSide = Domain::Block::GroundSide::Up
        );
        
        /**
         * groundSideに基づいてオブジェクトを分割
         * 1つのオブジェクトから壁・床・複数オブジェクトを生成
         */
        static Array<Domain::Block::BlockObject> expandObjectByGroundSide(const Domain::Block::BlockObject& obj);
    };
}

