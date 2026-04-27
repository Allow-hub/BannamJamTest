#pragma once
#include <memory>
#include "Domain/Block/IBlock.h"
#include "Domain/Block/BlockTypes.h"
#include "Domain/Physics/IPhysicsBody.h"
#include "Domain/Physics/PhysicsBodyID.h"
#include "Infrastructure/IPhysicsBodyFactory.h"
#include "Domain/Events/GameEvents.h"

namespace Jam::UseCase {
    /**
     * ステージサービス
     * ステージの配列管理と更新を担当
     * 物理ボディも別途管理し、同期処理を実施
     */
    class BlockService {
    private:
        Array<std::unique_ptr<Domain::Block::IBlock>> m_stages;
        Array<std::shared_ptr<Domain::Physics::IPhysicsBody>> m_physicsBodies;
        // 各ステージに対応する物理ボディのインデックス
        Array<Array<size_t>> m_bodyIndices;
        // 各物理ボディの基準位置からのオフセット
        Array<Vec2> m_bodyOffsets;
        // 各物理ボディのGroundSide情報（デバッグ描画用）
        Array<Domain::Block::GroundSide> m_bodyGroundSides;
        
    public:
        /**
         * ステージの初期化
         * @param filename ステージファイル名
         * @param bodyFactory 物理ボディファクトリー
         * @param eventQueue イベントキュー（ダメージ床用）
         * @param playerId プレイヤーID（ダメージ床用）
         * @return 初期化成功したか
         */
        bool initialize(
            const String& filename,
            std::shared_ptr<Infrastructure::Locator::IPhysicsBodyFactory> bodyFactory,
            Domain::Events::GameEventQueue& eventQueue,
            Domain::Physics::PhysicsBodyID playerId
        );
        
        /**
         * 全ステージの更新
         */
        void update(double deltaTime);
        
        /**
         * ステージの位置を物理ボディに同期
         */
        void syncPhysicsBodies();
        
        /**
         * ステージ配列の取得（読み取り専用）
         */
        const Array<std::unique_ptr<Domain::Block::IBlock>>& getStages() const;
        
        /**
         * 物理ボディ配列の取得（読み取り専用）
         */
        const Array<std::shared_ptr<Domain::Physics::IPhysicsBody>>& getPhysicsBodies() const;
        
        /**
         * ステージのクリア
         */
        void clear();
        
        /**
         * すり抜け床への着地判定と処理
         * @param playerBody プレイヤーの物理ボディ
         * @param isPressingDown 下ボタンが押されているか
         * @param playerHeight プレイヤーの高さ
         * @return 着地した場合true
         */
        bool checkOneWayPlatformLanding(
            std::shared_ptr<Domain::Physics::IPhysicsBody> playerBody,
            bool isPressingDown = false,
            double playerHeight = 100.0
        );
        
        /**
         * 物理レイヤー可視化のデバッグ描画
         */
        void drawPhysicsLayerDebug() const;
    };
}
