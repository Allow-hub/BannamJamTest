# pragma once
# include "ProjectileBase.h"

class Bomb final : public ProjectileBase
{
private:

	// 物理ボディ
	P2Body m_body;

	// 描画用の色
	ColorF m_color = Palette::Orange;

	// 爆発エフェクト用のタイマー（ヒット後に使用）
	Stopwatch m_hitEffectTimer{ StartImmediately::No };

	double m_radius;

	/// @brief 軌道計算に必要な初速を計算する
	/// @param from 発射地点
	/// @param to 目標地点
	/// @param gravityY Y軸の重力
	/// @param peakHeight 軌道の最高点（発射地点からの相対）
	/// @return 計算された初速 (Vec2)
	Vec2 CalculateBombVelocity(const Vec2& from, const Vec2& to, double gravityY, double peakHeight);

public:

	/// @brief コンストラクタ
	/// @param world 物理ワールド
	/// @param enemyPos 発射地点（エネミーの位置）
	/// @param playerPos 目標地点（プレイヤーの位置）
	Bomb(P2World& world, const Vec2& enemyPos, const Vec2& playerPos);

	void update(P2World& world) override;

	void draw() const override;
};
