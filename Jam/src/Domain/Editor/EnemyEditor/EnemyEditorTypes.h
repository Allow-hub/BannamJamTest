#pragma once
#include <Siv3D.hpp>
#include "../../Enemy/EnemyType.h"
namespace Jam::Domain::Editor
{
    // 巡回ポイント
    struct PatrolPoint
    {
        Vec2 position;
    };

    // AI巡回設定
    struct PatrolAI
    {
        Array<PatrolPoint> patrolPoints;
        bool loop = true;
        double waitTime = 1.0;
        double foundDistance = 700.0;
    };

    // AI追跡設定
    struct ChaseAI
    {
        double attackRange = 600.0;
        double loseRange = 700.0;
        double moveSpeedFactor = 1.2;
    };

    // 敵オブジェクト
    struct EnemyObject
    {
        Domain::EnemyType type = Domain::EnemyType::LittleDevil;
        Vec2 position{0, 0};
        
        // AI設定（LittleDevilは巡回のみ、それ以外は追跡も持つ）
        PatrolAI patrol;
        Optional<ChaseAI> chase;
        
        // 追跡AIを持つか判定
        bool hasChaseAI() const
        {
            return type != Domain::EnemyType::LittleDevil && type != Domain::EnemyType::Boss1_3;
        }
        
        // ボスか判定
        bool isBoss() const
        {
            return type == Domain::EnemyType::Boss1_3;
        }
    };
}