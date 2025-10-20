#pragma once
#include <Siv3D.hpp>
#include "../Domain/Physics/PhysicsTypes.h"

namespace Jam::Infrastructure
{
	enum class PhysicsFilter
	{
		Wall,
		Team1,
		Team2,
		Attack,
		Team1Ghost,  // 攻撃中に敵を貫通するプレイヤー
	};

	// === 基本的な層ビット定義 ===
	constexpr uint16 WallBit = 0b0000'0000'0000'0001;
	constexpr uint16 Team1Bit = 0b0000'0000'0000'0010;
	constexpr uint16 Team2Bit = 0b0000'0000'0000'0100;
	constexpr uint16 AttackBit = 0b0000'0000'0000'1000;

	// === 個別フィルタ定義 ===
	constexpr P2Filter WallFilter{
		.categoryBits = WallBit,
		.maskBits = 0xFFFF // 全部に当たる
	};

	constexpr P2Filter Team1Filter{
		.categoryBits = Team1Bit,
		.maskBits = WallBit | Team2Bit // 壁と敵に当たる
	};

	constexpr P2Filter Team2Filter{
		.categoryBits = Team2Bit,
		.maskBits = WallBit | Team1Bit // 壁とプレイヤーに当たる
	};

	constexpr P2Filter AttackFilter{
		.categoryBits = AttackBit,
		.maskBits = Team1Bit | Team2Bit // 両チームにヒット可能
	};

	// プレイヤーが敵を貫通する状態
	constexpr P2Filter Team1GhostFilter{
		.categoryBits = Team1Bit,
		.maskBits = Team1Bit 
	};

	// === Enum から Filter を取得するユーティリティ ===
	inline constexpr const P2Filter& GetFilter(PhysicsFilter layer)
	{
		switch (layer)
		{
		case PhysicsFilter::Wall:       return WallFilter;
		case PhysicsFilter::Team1:      return Team1Filter;
		case PhysicsFilter::Team2:      return Team2Filter;
		case PhysicsFilter::Attack:     return AttackFilter;
		case PhysicsFilter::Team1Ghost: return Team1GhostFilter;
		default:                       return WallFilter;
		}
	}
}
