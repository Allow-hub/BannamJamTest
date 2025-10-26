# include "ProjectileManager.h"

ProjectileManager::ProjectileManager()
{
	// 物理ワールドの重力を設定（例: (0, 980)）
	m_world.setGravity(Vec2{ 0, 980 });
}

void ProjectileManager::createProjectile(ProjectileType type, const Vec2& enemyPos, const Vec2& playerPos)
{
	// ファクトリに生成を依頼
	auto projectile = m_factory.create(type, m_world, enemyPos, playerPos);

	if (projectile) // 生成が成功したら
	{
		// 管理リストに追加
		m_projectiles.push_back(std::move(projectile));
	}
}

void ProjectileManager::update()
{
	// 1. 物理ワールドの更新
	// (Bombがこれによって動く)
	m_world.update();

	// 2. 全ての飛翔物の update を呼び出す
	for (auto& p : m_projectiles)
	{
		p->update(m_world);
	}

	// 3. isAlive() == false になった飛翔物をリストから削除
	m_projectiles.remove_if([](const std::unique_ptr<ProjectileBase>& p)
	{
		if (p->isAlive())
		{
			return false; // 生きているので削除しない
		}

		// ----- 攻撃処理への連携 -----
		// 本来はここで「攻撃処理クラス」に通知します
		if (p->getState() == ProjectileState::Hit)
		{
			// (例) AttackManager::GetInstance().RegisterHit(p->getAttackInfo());
			// 今回は、状態がHitになったことをコンソールに出力してみます
			Print << U"Projectile Hit!";
		}

		return true; // 削除する
	});
}

void ProjectileManager::draw() const
{
	// 全ての飛翔物の draw を呼び出す
	for (const auto& p : m_projectiles)
	{
		p->draw();
	}

	// (デバッグ用) 物理ボディの輪郭を描画
	// m_world.drawDebug();
}
