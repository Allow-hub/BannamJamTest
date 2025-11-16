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
     * 繧ｹ繝・・繧ｸ逕滓・邨先棡
     * 繧ｹ繝・・繧ｸ縺ｨ迚ｩ逅・・繝・ぅ繧貞・髮｢縺励※邂｡逅・
     */
    struct StageCreationResult {
        Array<std::unique_ptr<Domain::Stage::IStage>> stages;
        Array<std::shared_ptr<Domain::Physics::IPhysicsBody>> physicsBodies;
        // 蜷・せ繝・・繧ｸ縺ｫ蟇ｾ蠢懊☆繧狗黄逅・・繝・ぅ縺ｮ繧､繝ｳ繝・ャ繧ｯ繧ｹ驟榊・
        // stages[i]縺ｯphysicsBodies[bodyIndices[i][0], bodyIndices[i][1], ...]繧貞宛蠕｡
        Array<Array<size_t>> bodyIndices;
        // 蜷・黄逅・・繝・ぅ縺ｮ蝓ｺ貅紋ｽ咲ｽｮ縺九ｉ縺ｮ繧ｪ繝輔そ繝・ヨ
        Array<Vec2> bodyOffsets;
        // 蜷・黄逅・・繝・ぅ縺ｮGroundSide諠・ｱ・医ョ繝舌ャ繧ｰ謠冗判逕ｨ・・
        Array<Domain::Stage::GroundSide> bodyGroundSides;
    };
    
    /**
     * 繧ｹ繝・・繧ｸ繝輔ぃ繧ｯ繝医Μ繝ｼ
     * JSON繝輔ぃ繧､繝ｫ縺九ｉ繧ｹ繝・・繧ｸ繧ｪ繝悶ず繧ｧ繧ｯ繝医・驟榊・繧堤函謌・
     */
    class StageFactory {
    public:
        /**
         * JSON繝輔ぃ繧､繝ｫ縺九ｉ蜈ｨ繧ｹ繝・・繧ｸ繧堤函謌・
         * @param filename 繧ｹ繝・・繧ｸ繝輔ぃ繧､繝ｫ蜷・
         * @param bodyFactory 迚ｩ逅・・繝・ぅ繝輔ぃ繧ｯ繝医Μ繝ｼ
         * @param eventQueue 繧､繝吶Φ繝医く繝･繝ｼ・医ム繝｡繝ｼ繧ｸ蠎顔畑・・
         * @param playerId 繝励Ξ繧､繝､繝ｼID・医ム繝｡繝ｼ繧ｸ蠎顔畑・・
         * @return 逕滓・縺輔ｌ縺溘せ繝・・繧ｸ縺ｨ迚ｩ逅・・繝・ぅ
         */
        static StageCreationResult createStagesFromFile(
            const String& filename,
            std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory,
            Domain::Events::GameEventQueue& eventQueue,
            Domain::Physics::PhysicsBodyID playerId
        );
        
    private:
        /**
         * 蛟句挨縺ｮ繧ｹ繝・・繧ｸ繧ｪ繝悶ず繧ｧ繧ｯ繝医ｒ逕滓・
         * @param obj 繧ｹ繝・・繧ｸ繧ｪ繝悶ず繧ｧ繧ｯ繝医ョ繝ｼ繧ｿ
         * @param bodyFactory 迚ｩ逅・・繝・ぅ繝輔ぃ繧ｯ繝医Μ繝ｼ
         * @param outBody 逕滓・縺輔ｌ縺溽黄逅・・繝・ぅ・亥・蜉幢ｼ・
         * @param eventQueue 繧､繝吶Φ繝医く繝･繝ｼ・医ム繝｡繝ｼ繧ｸ蠎顔畑・・
         * @param playerId 繝励Ξ繧､繝､繝ｼID・医ム繝｡繝ｼ繧ｸ蠎顔畑・・
         * @return 逕滓・縺輔ｌ縺溘せ繝・・繧ｸ
         */
        static std::unique_ptr<Domain::Stage::IStage> createStage(
            const Domain::Stage::StageObject& obj,
            std::shared_ptr<Locator::IPhysicsBodyFactory> bodyFactory,
            std::shared_ptr<Domain::Physics::IPhysicsBody>& outBody,
            Domain::Events::GameEventQueue& eventQueue,
            Domain::Physics::PhysicsBodyID playerId
        );
        
        /**
         * StageType縺ｨGroundSide縺九ｉ驕ｩ蛻・↑迚ｩ逅・Ξ繧､繝､繝ｼ繧貞叙蠕・
         */
        static Domain::Physics::PhysicsLayer getPhysicsLayerFromType(
            Domain::Stage::StageType type,
            Domain::Stage::GroundSide groundSide = Domain::Stage::GroundSide::Up
        );
        
        /**
         * groundSide縺ｫ蝓ｺ縺･縺・※繧ｪ繝悶ず繧ｧ繧ｯ繝医ｒ螻暮幕
         * 1縺､縺ｮ繧ｪ繝悶ず繧ｧ繧ｯ繝医°繧牙｣・蠎翫・隍・焚繧ｪ繝悶ず繧ｧ繧ｯ繝医ｒ逕滓・
         */
        static Array<Domain::Stage::StageObject> expandObjectByGroundSide(const Domain::Stage::StageObject& obj);
    };
}

