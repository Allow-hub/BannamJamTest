#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Editor
{
    // 謨ｵ縺ｮ遞ｮ鬘・
    enum class EnemyType
    {
        LittleDevil,    // 蟆乗が鬲・
        Spider,         // 陷倩屁
        Ribbon,         // 繝ｪ繝懊Φ
        Eye,            // 逶ｮ邇・
        Clown,          // 驕灘喧蟶ｫ
        Boss1_3         // 繝懊せ
    };

    // 謨ｵ縺ｮ遞ｮ鬘槭ｒ譁・ｭ怜・縺ｫ螟画鋤
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

    // 譁・ｭ怜・繧呈雰縺ｮ遞ｮ鬘槭↓螟画鋤
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

    // 蟾｡蝗槭・繧､繝ｳ繝・
    struct PatrolPoint
    {
        Vec2 position;
    };

    // AI蟾｡蝗櫁ｨｭ螳・
    struct PatrolAI
    {
        Array<PatrolPoint> patrolPoints;
        bool loop = true;
        double waitTime = 1.0;
        double foundDistance = 700.0;
    };

    // AI霑ｽ霍｡險ｭ螳・
    struct ChaseAI
    {
        double attackRange = 600.0;
        double loseRange = 700.0;
        double moveSpeedFactor = 1.2;
    };

    // 謨ｵ繧ｪ繝悶ず繧ｧ繧ｯ繝・
    struct EnemyObject
    {
        EnemyType type = EnemyType::LittleDevil;
        Vec2 position{0, 0};
        
        // AI險ｭ螳夲ｼ・ittleDevil縺ｯ蟾｡蝗槭・縺ｿ縲√◎繧御ｻ･螟悶・霑ｽ霍｡繧よ戟縺､・・
        PatrolAI patrol;
        Optional<ChaseAI> chase;
        
        // 霑ｽ霍｡AI繧呈戟縺､縺句愛螳・
        bool hasChaseAI() const
        {
            return type != EnemyType::LittleDevil && type != EnemyType::Boss1_3;
        }
        
        // 繝懊せ縺句愛螳・
        bool isBoss() const
        {
            return type == EnemyType::Boss1_3;
        }
    };

    // 繧ｨ繝・ぅ繧ｿ逕ｨ縺ｮ謨ｵ繧ｪ繝悶ず繧ｧ繧ｯ繝・
    struct EnemyEditorObject
    {
        Optional<size_t> id;
        EnemyObject enemyObject;
        bool isSelected = false;
    };

    // Undo/Redo逕ｨ繧ｳ繝槭Φ繝・
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
