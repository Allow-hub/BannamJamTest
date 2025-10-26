# pragma once
# include <Siv3D.hpp>
# include "ProjectileBase.h"
# include "ProjectileFactory.h"
# include "ProjectileType.h"

namespace Jam::Domain::Projectile
{
	class ProjectileManager
	{
	private:

		// 飛翔物のリスト（スマートポインタで管理）
		Array<std::unique_ptr<ProjectileBase>> m_projectiles;

		// 飛翔物ファクトリ
		ProjectileFactory m_factory;

		// 物理演算ワールド (Bombが使用)
		P2World m_world;

	public:

		ProjectileManager();

		/// @brief 飛翔物を生成し、管理下に追加する（エネミーから呼び出す）
		/// @param type 生成する飛翔物の種類
		/// @param enemyPos 発射地点
		/// @param playerPos 目標地点
		void createProjectile(ProjectileType type, const Vec2& enemyPos, const Vec2& playerPos);

		/// @brief 管理している全ての飛翔物を更新する
		void update();

		/// @brief 管理している全ての飛翔物を描画する
		void draw() const;
	};

}
