#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Editor
{
    // 敵の種類
    enum class EnemyType
    {
        LittleDevil,    // 小悪魔
        Spider,         // 蜘蛛
        Ribbon,         // リボン
        Eye,            // 目玉
        Clown,          // 道化師
        Boss1_3         // ボス
    };

    // 敵の種類を文字列に変換
    inline String enemyTypeToString(EnemyType type)
    {
        switch (type)
        {
        case EnemyType::LittleDevil: return U"LittleDevil";
        case EnemyType::Spider: return U"Spider";
        case EnemyType::Ribbon: return U"Ribbon";
        case EnemyType::Eye: return U"Eye";
        case EnemyType::Clown: return U"Clown";
        case EnemyType::Boss1_3: return U"Boss1_3";
        default: return U"LittleDevil";
        }
    }

    // 文字列を敵の種類に変換
    inline EnemyType stringToEnemyType(const String& str)
    {
        if (str == U"LittleDevil") return EnemyType::LittleDevil;
        if (str == U"Spider") return EnemyType::Spider;
        if (str == U"Ribbon") return EnemyType::Ribbon;
        if (str == U"Eye") return EnemyType::Eye;
        if (str == U"Clown") return EnemyType::Clown;
        if (str == U"Boss1_3") return EnemyType::Boss1_3;
        return EnemyType::LittleDevil;
    }

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
        EnemyType type = EnemyType::LittleDevil;
        Vec2 position{0, 0};
        
        // AI設定（LittleDevilは巡回のみ、それ以外は追跡も持つ）
        PatrolAI patrol;
        Optional<ChaseAI> chase;
        
        // 追跡AIを持つか判定
        bool hasChaseAI() const
        {
            return type != EnemyType::LittleDevil && type != EnemyType::Boss1_3;
        }
        
        // ボスか判定
        bool isBoss() const
        {
            return type == EnemyType::Boss1_3;
        }
    };

    // エディタ用の敵オブジェクト
    struct EnemyEditorObject
    {
        Optional<size_t> id;
        EnemyObject enemyObject;
        bool isSelected = false;
    };

    // Undo/Redo用コマンド
    struct EnemyEditorCommand
    {
        enum class Type
        {
            Add,
            Delete,
            Modify
        };

        Type type;
        EnemyEditorObject object;
    };
}
