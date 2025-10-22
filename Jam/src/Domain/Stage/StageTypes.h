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
		Normal = 1,      // 壁・床（完全ブロック）
		MovingPlatform = 2,  // 動く足場
		Hazard = 3,     // ダメージゾーン
		Trigger = 4,    // イベントトリガー
		Breakable = 5,  // 破壊可能な壁
	};

	// ステージオブジェクト（床・壁・足場など）
	struct StageObject {
		RectF rect;                     // 位置・サイズ
		StageType type;                 // 当たり判定種別
		String metadata;                // 識別用ID（破壊時などで使用）

		// 動くプラットフォーム用の追加データ
		Vec2 movementSpeed = { 0, 0 };    // 移動速度
		Array<Vec2> movementPath;       // 移動パス
		bool loopMovement = true;       // パスをループするか

		// デフォルトコンストラクタ（JSONパース時の安全な初期化用）
		StageObject()
			: rect(0, 0, 0, 0)
			, type(StageType::None)
			, metadata(U"") {
		}
	};

	// 文字列からCollisionTypeへの変換（StageLoaderで使用）
	inline StageType stringToCollisionType(const String& typeStr) {
		if (typeStr == U"normal") return StageType::Normal;
		if (typeStr == U"moving_platform") return StageType::MovingPlatform;
		if (typeStr == U"hazard") return StageType::Hazard;
		if (typeStr == U"trigger") return StageType::Trigger;
		if (typeStr == U"breakable") return StageType::Breakable;
		return StageType::None;
	}

	// CollisionTypeから文字列への変換（デバッグ用）
	inline String collisionTypeToString(StageType type) {
		switch (type) {
		case StageType::Normal: return U"normal";
		case StageType::MovingPlatform: return U"moving_platform";
		case StageType::Hazard: return U"hazard";
		case StageType::Trigger: return U"trigger";
		case StageType::Breakable: return U"breakable";
		default: return U"none";
		}
	}
}
