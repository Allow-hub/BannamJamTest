# include "Bomb.h"

// 軌道計算のヘルパー関数
Vec2 Bomb::CalculateBombVelocity(const Vec2& from, const Vec2& to, double gravityY, double peakHeight)
{
	const double displacementY = to.y - from.y;
	const Vec2 displacementXZ{ to.x - from.x, 0.0 };

	// 目的地点までの時間 (Y方向の計算)
	// peakHeight は from.y からの相対的な高さとする
	const double time = Sqrt(-2.0 * peakHeight / gravityY) + Sqrt(2.0 * (displacementY - peakHeight) / gravityY);

	//XZ方向の速度
	const Vec2 velocityXZ = displacementXZ / time;

	//Y方向の速度
	const double velocityY = Sqrt(-2.0 * gravityY * peakHeight);

	return{ velocityXZ.x, -velocityY };
}

Bomb::Bomb(P2World& world, const Vec2& enemyPos, const Vec2& playerPos)
{
	// 物理ボディの半径
	m_radius = 10.0;

	// 物理演算ワールドの重力 (P2Worldのデフォルトは (0, 980) です)
	const double gravityY = world.getGravity().y;

	// 軌道の高さ（エネミーとプレイヤーの距離に応じて調整するとそれらしくなります）
	const double peakHeight = Clamp(Abs(enemyPos.x - playerPos.x) * 0.5, 50.0, 300.0);

	// 初速を計算
	const Vec2 velocity = CalculateBombVelocity(enemyPos, playerPos, gravityY, -peakHeight); // Y軸が逆なので-peakHeight

	P2Material material;
	material.restitution = 0.3;

	// 物理ボディを生成
	m_body = world.createCircle(P2BodyType::Dynamic, enemyPos, m_radius, material);
	m_body.setVelocity(velocity); // 計算した初速を適用
}

void Bomb::update(P2World& world)
{
	// すでにHit状態なら
	if (m_state == ProjectileState::Hit)
	{
		// 爆発エフェクトが終了したら消滅
		if (m_hitEffectTimer.sF() > 0.5) // 0.5秒で消滅
		{
			m_isAlive = false;
		}
		return;
	}

	// 簡易的な衝突判定 (本来は P2Body の衝突コールバックを使いますが、ここでは簡略化)
	// プレイヤーや壁との衝突を想定
	// ここでは仮に「画面下端」に当たったらHitとする
	if (m_body.getPos().y > (Scene::Height() - m_radius))
	{
		m_state = ProjectileState::Hit;
		m_hitEffectTimer.start(); // 爆発エフェクトタイマースタート

		// ヒットしたら物理ボディをワールドから削除
		m_body.release();
	}
}

void Bomb::draw() const
{
	if (m_state == ProjectileState::Updating)
	{
		// 更新中：物理ボディの位置に円を描画
		m_body.draw(m_color);
	}
	else if (m_state == ProjectileState::Hit)
	{
		// Hit状態：簡易的な爆発エフェクト（円が広がる）
		// (攻撃判定の生成は「攻撃クラス」の役割)
		const double r = m_hitEffectTimer.sF() * 100.0; // 時間経過で円が広がる
		Circle(m_body.getPos(), r).draw(ColorF(m_color, 0.5));
	}
}
