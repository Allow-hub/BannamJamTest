#pragma once
#include <Siv3D.hpp>
#include "../Domain/Physics/PhysicsTypes.h"

namespace Jam::Infrastructure
{
	enum class PhysicsFilter
	{
		Wall,
		SlipThrough,//プレイヤーと干渉しない（未設定）
		Team1,//プレイヤー系
		Team2,//敵
		Team2Death,//死亡時の敵
		Attack,
		Team1Ghost,  // 攻撃中に敵を貫通するプレイヤー
		Item
	};
		
	// === 基本的な層ビット定義 ===
	constexpr uint16 WallBit = 0b0000'0000'0000'0001;
	constexpr uint16 Team1Bit = 0b0000'0000'0000'0010;
	constexpr uint16 Team2Bit = 0b0000'0000'0000'0100;
	constexpr uint16 AttackBit = 0b0000'0000'0000'1000;
	constexpr uint16 ItemBit = 0b0000'0000'0001'0000;
	
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
		.maskBits = Team2Bit | WallBit | Team1Bit // 敵が敵と壁とプレイヤーに当たる
	};

	constexpr P2Filter Team2DeathFilter{
	.categoryBits = Team2Bit,
	.maskBits = Team2Bit | WallBit // 壁と敵に当たるだけ
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
	constexpr P2Filter ItemFilter{
	.categoryBits = ItemBit,
	.maskBits = Team1Bit // プレイヤーとだけ接触
	};

	// === Enum から Filter を取得するユーティリティ ===
	inline constexpr const P2Filter& GetFilter(PhysicsFilter layer)
	{
		switch (layer)
		{
		case PhysicsFilter::Wall:       return WallFilter;
		case PhysicsFilter::Team1:      return Team1Filter;
		case PhysicsFilter::Team2:      return Team2Filter;
		case PhysicsFilter::Team2Death:   return Team2DeathFilter;
		case PhysicsFilter::Attack:     return AttackFilter;
		case PhysicsFilter::Team1Ghost: return Team1GhostFilter;
		case PhysicsFilter::Item:         return ItemFilter;
		default:                       return WallFilter;
		}
	}
}
