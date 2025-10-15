#pragma once
#include <Siv3D.hpp>

/**
 * ステージ関連の型定義
 * 当たり判定種別とステージオブジェクト構造体
 */
namespace Jam::Domain::Stage {
    // 当たり判定の種類
    enum class StageType {
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
        StageType type;             // 当たり判定種別
        String metadata;                // 識別用ID（破壊時などで使用）
        
        // デフォルトコンストラクタ（JSONパース時の安全な初期化用）
        StageObject() 
            : rect(0, 0, 0, 0)
            , type(StageType::None)
            , metadata(U"") {}
    };

    // 文字列からCollisionTypeへの変換（StageLoaderで使用）
    inline StageType stringToCollisionType(const String& typeStr) {
        if (typeStr == U"solid") return StageType::Solid;
        if (typeStr == U"platform") return StageType::Platform;
        if (typeStr == U"hazard") return StageType::Hazard;
        if (typeStr == U"trigger") return StageType::Trigger;
        if (typeStr == U"breakable") return StageType::Breakable;
        return StageType::None;
    }

    // CollisionTypeから文字列への変換（デバッグ用）
    inline String collisionTypeToString(StageType type) {
        switch (type) {
        case StageType::Solid: return U"solid";
        case StageType::Platform: return U"platform";
        case StageType::Hazard: return U"hazard";
        case StageType::Trigger: return U"trigger";
        case StageType::Breakable: return U"breakable";
        default: return U"none";
        }
    }
}