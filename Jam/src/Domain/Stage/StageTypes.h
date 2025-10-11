#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Stage {
    // 当たり判定の種類
    enum class CollisionType {
        None = 0,
        Solid = 1,      // 壁・床
        Platform = 2,   // 足場
        Hazard = 3,     // ダメージゾーン
        Trigger = 4     // イベントトリガー
    };

    // ステージオブジェクト（床・壁・足場など）
    struct StageObject {
        RectF rect;           // 位置・サイズ
        CollisionType type;   // 当たり判定種別
        Color color;          // 描画色
        String metadata;      // 追加情報
    };

    // ステージ全体の情報
    struct StageInfo {
        String id;                    // ステージID
        String name;                  // ステージ名
        Size size;                    // ステージサイズ
        Array<StageObject> objects;   // ステージオブジェクト一覧
    };
}