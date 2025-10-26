#pragma once
#include <Siv3D.hpp>
#include "../Domain/Physics/PhysicsTypes.h"

namespace Jam::Infrastructure
{
	enum class PhysicsFilter
	{
		Wall,
		SlipThrough,      // プレイヤーと干渉しない（未設定）
		Team1,            // プレイヤー系
		Team2,            // 敵
		Team2Death,       // 死亡時の敵
		Attack,
		Team1Ghost,  // 攻撃中に敵を貫通するプレイヤー
		OneWayPlatform,  // すり抜ける床
		Item,
		PlayerWeapon,     // プレイヤーの武器
		EnemyWeapon       // 敵の武器
	};

	// === 基本的な層ビット定義 ===
	constexpr uint16 WallBit = 0b0000'0000'0000'0001;
	constexpr uint16 Team1Bit = 0b0000'0000'0000'0010;
	constexpr uint16 Team2Bit = 0b0000'0000'0000'0100;
	constexpr uint16 AttackBit = 0b0000'0000'0000'1000;
	constexpr uint16 OneWayBit = 0b0000'0000'0001'0000;
	constexpr uint16 ItemBit = 0b0000'0000'0001'0000;
	constexpr uint16 PlayerWeaponBit = 0b0000'0000'0010'0000;
	constexpr uint16 EnemyWeaponBit = 0b0000'0000'0100'0000;

	// === 個別フィルタ定義 ===

	// 壁：全部に当たる
	constexpr P2Filter WallFilter{
		.categoryBits = WallBit,
		.maskBits = 0xFFFF
	};

	// プレイヤー：壁、敵、敵の武器に当たる
	constexpr P2Filter Team1Filter{
		.categoryBits = Team1Bit,
		.maskBits = WallBit | Team2Bit | EnemyWeaponBit
	};

	// 敵：敵、壁、プレイヤー、プレイヤーの武器に当たる
	constexpr P2Filter Team2Filter{
		.categoryBits = Team2Bit,
		.maskBits = Team2Bit | WallBit | Team1Bit | PlayerWeaponBit
	};

	// 死亡時の敵：壁と敵に当たるだけ
	constexpr P2Filter Team2DeathFilter{
		.categoryBits = Team2Bit,
		.maskBits = Team2Bit | WallBit
	};

	// プレイヤーゴースト：敵を貫通する
	constexpr P2Filter Team1GhostFilter{
		.categoryBits = Team1Bit,
		.maskBits = WallBit
	};

	// すり抜け床（プレイヤーと衝突しない）
	constexpr P2Filter OneWayPlatformFilter{
		.categoryBits = OneWayBit,
		.maskBits = 0x0000  // 誰とも衝突しない
	};

	// アイテム：プレイヤーとだけ接触
	constexpr P2Filter ItemFilter{
		.categoryBits = ItemBit,
		.maskBits = Team1Bit
	};

	// プレイヤーの武器：壁と敵に当たる（敵の武器とは当たらない）
	constexpr P2Filter PlayerWeaponFilter{
		.categoryBits = PlayerWeaponBit,
		.maskBits = WallBit | Team2Bit
	};

	// 敵の武器：壁とプレイヤーに当たる（プレイヤーの武器とは当たらない）
	constexpr P2Filter EnemyWeaponFilter{
		.categoryBits = EnemyWeaponBit,
		.maskBits = WallBit | Team1Bit
	};

	// === Enum から Filter を取得するユーティリティ ===
	inline constexpr const P2Filter& GetFilter(PhysicsFilter layer)
	{
		switch (layer)
		{
		case PhysicsFilter::Wall:          return WallFilter;
		case PhysicsFilter::Team1:         return Team1Filter;
		case PhysicsFilter::Team2:         return Team2Filter;
		case PhysicsFilter::Team2Death:    return Team2DeathFilter;
		case PhysicsFilter::Team1Ghost:    return Team1GhostFilter;
		case PhysicsFilter::OneWayPlatform: return OneWayPlatformFilter;
		case PhysicsFilter::Item:          return ItemFilter;
		case PhysicsFilter::PlayerWeapon:  return PlayerWeaponFilter;
		case PhysicsFilter::EnemyWeapon:   return EnemyWeaponFilter;
		default:                           return WallFilter;
		}
	}
}
