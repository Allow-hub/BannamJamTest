#pragma once
#include "ProjectileBase.h"

class FireBoal : public ProjectileBase {
private:
	s3d::Vec2 m_position; //現在位置
	s3d::Vec2 m_velocity; //速度
	s3d::Stopwatch m_stopwatch; //生存時間タイマー
	double m_lifeTimeSec; //消滅までの時間

public:
	// コンストラクタ
	FireBoal(const s3d::Vec2& startPos, const s3d::Vec2& targetPos, double lifeTime = 3.0);

	// 基底クラスの関数をオーバーライド
	ProjectileState update(double deltaTime) override;
	void draw() const override;
};
