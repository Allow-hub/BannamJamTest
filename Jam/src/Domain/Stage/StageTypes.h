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
		Normal = 1,          // 壁・床(完全ブロック)
		MovingPlatform = 2,  // 動く足場
		OneWayPlatform = 3,  // すり抜ける床(下から上に通過可能)
		DamagePlatform = 4,  // ダメージを与える床
		MovingDamagePlatform = 5,  // 動いてダメージを与える床
	};

	// 動く床の移動タイプ
	enum class MovementType {
		Horizontal,  // 横移動
		Vertical,    // 縦移動
		Circular     // 円運動
	};

	// 地上判定を付ける面
	enum class GroundSide {
		None,   // 地上判定なし（全面Wall）
		Up,   // 上面のみGround
		Down,
		Left,
		Right,
		All     // 全面Ground（デフォルト）
	};

	// ステージオブジェクト(床・壁・足場など)
	struct StageObject {
		RectF rect;                     // 位置・サイズ
		StageType type;                 // 当たり判定種別
		String metadata;                // 識別用ID(破壊時などで使用)
		GroundSide groundSide = GroundSide::Up;  // 地上判定を付ける面

		// 動くプラットフォーム用の追加データ
		MovementType movementType = MovementType::Horizontal; // 移動タイプ
		double movementSpeed = 100.0;   // 移動速度(ピクセル/秒)
		double movementDistance = 200.0; // 移動距離(横・縦)または半径(円)
		bool loopMovement = true;       // パスをループするか

		// ダメージプラットフォーム用の追加データ
		double damageAmount = 10.0;     // 与えるダメージ量

		// デフォルトコンストラクタ(JSONパース時の安全な初期化用)
		StageObject()
			: rect(0, 0, 0, 0)
			, type(StageType::None)
			, metadata(U"") {
		}
	};

	// 文字列からCollisionTypeへの変換(StageLoaderで使用)
	inline StageType stringToCollisionType(const String& typeStr) {
		if (typeStr == U"normal" || typeStr == U"solid") return StageType::Normal;
		if (typeStr == U"moving_platform" || typeStr == U"moving" || typeStr == U"move") return StageType::MovingPlatform;
		if (typeStr == U"oneway_platform" || typeStr == U"oneway") return StageType::OneWayPlatform;
		if (typeStr == U"damage_platform" || typeStr == U"damage") return StageType::DamagePlatform;
		if (typeStr == U"moving_damage_platform" || typeStr == U"movingDamage") return StageType::MovingDamagePlatform;
		return StageType::None;
	}

	// 文字列からMovementTypeへの変換
	inline MovementType stringToMovementType(const String& typeStr) {
		if (typeStr == U"horizontal") return MovementType::Horizontal;
		if (typeStr == U"vertical") return MovementType::Vertical;
		if (typeStr == U"circular") return MovementType::Circular;
		return MovementType::Horizontal;
	}

	// 文字列からGroundSideへの変換
	inline GroundSide stringToGroundSide(const String& sideStr) {
		if (sideStr == U"none") return GroundSide::None;
		if (sideStr == U"up") return GroundSide::Up;
		if (sideStr == U"all") return GroundSide::All;
		return GroundSide::All;  // デフォルトは全面Ground
	}

	// CollisionTypeから文字列への変換(デバッグ用)
	inline String collisionTypeToString(StageType type) {
		switch (type) {
		case StageType::Normal: return U"normal";
		case StageType::MovingPlatform: return U"moving_platform";
		case StageType::OneWayPlatform: return U"oneway_platform";
		case StageType::DamagePlatform: return U"damage_platform";
		case StageType::MovingDamagePlatform: return U"moving_damage_platform";
		default: return U"none";
		}
	}

	// MovementTypeから文字列への変換
	inline String movementTypeToString(MovementType type) {
		switch (type) {
		case MovementType::Horizontal: return U"horizontal";
		case MovementType::Vertical: return U"vertical";
		case MovementType::Circular: return U"circular";
		default: return U"horizontal";
		}
	}

	// GroundSideから文字列への変換
	inline String groundSideToString(GroundSide side) {
		switch (side) {
		case GroundSide::None: return U"none";
		case GroundSide::Up: return U"up";
		case GroundSide::Down: return U"down";
		case GroundSide::Left: return U"left";
		case GroundSide::Right: return U"right";
		case GroundSide::All: return U"all";
		default: return U"all";
		}
	}
}
