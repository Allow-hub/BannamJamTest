#pragma once
#include <Siv3D.hpp>
#include "../../Enemy/EnemyType.h"
#include "../../Enemy/EnemyAI/AISettings.h"
namespace Jam::Domain::Editor
{
    // エディター用巡回ポイント
    struct PatrolPoint
    {
        Vec2 position;
    };

    // エディター用巡回データ（エディター固有のデータのみ）
    struct PatrolData
    {
        Array<PatrolPoint> patrolPoints;
    };

    // 敵オブジェクト
    struct EnemyObject
    {
        Domain::EnemyType type = Domain::EnemyType::LittleDevil;
        Vec2 position{0, 0};
        
        // エディター用データ（LittleDevilは巡回のみ、それ以外は追跡も持つ）
        PatrolData patrol;
        
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