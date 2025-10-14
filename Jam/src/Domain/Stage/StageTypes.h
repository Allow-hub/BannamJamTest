#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Stage {
    // 当たり判定の種類
    enum class CollisionType {
        None = 0,
        Solid = 1,      // 壁・床（完全ブロック）
        Platform = 2,   // 足場（上からのみ乗れる）
        Hazard = 3,     // ダメージゾーン
        Trigger = 4,    // イベントトリガー
        Breakable = 5   // 破壊可能な壁
    };

    // ステージオブジェクト（床・壁・足場など）
    struct StageObject {
        RectF rect;                     // 位置・サイズ
        CollisionType type;             // 当たり判定種別
        Color color;                    // 描画色
        String metadata;                // 識別用ID（破壊時などで使用）
        bool destructible = false;      // 破壊可能フラグ
        
        // デフォルトコンストラクタ（JSONパース時の安全な初期化用）
        StageObject() 
            : rect(0, 0, 0, 0)
            , type(CollisionType::None)
            , color(Palette::Gray)
            , metadata(U"") {}
    };

    // 文字列からCollisionTypeへの変換（StageLoaderで使用）
    inline CollisionType stringToCollisionType(const String& typeStr) {
        if (typeStr == U"solid") return CollisionType::Solid;
        if (typeStr == U"platform") return CollisionType::Platform;
        if (typeStr == U"hazard") return CollisionType::Hazard;
        if (typeStr == U"trigger") return CollisionType::Trigger;
        if (typeStr == U"breakable") return CollisionType::Breakable;
        return CollisionType::None;
    }

    // CollisionTypeから文字列への変換（デバッグ用）
    inline String collisionTypeToString(CollisionType type) {
        switch (type) {
        case CollisionType::Solid: return U"solid";
        case CollisionType::Platform: return U"platform";
        case CollisionType::Hazard: return U"hazard";
        case CollisionType::Trigger: return U"trigger";
        case CollisionType::Breakable: return U"breakable";
        default: return U"none";
        }
    }
}