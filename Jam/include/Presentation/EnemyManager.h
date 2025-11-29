#pragma once
#include <Siv3D.hpp>
#include <unordered_map>
#include <memory>
#include "../Domain/Enemy/EnemyBase.h"
#include "Animator.h"

namespace Jam::Presentation
{
	/// @brief 敵オブジェクトとアニメーターを管理するクラス
	class EnemyManager
	{
	public:
		/// @brief 敵を追加する
		/// @param enemy 追加するEnemyオブジェクト
		/// @param animJsonPath アニメーター設定JSONパス
		/// @return 割り当てられたID
		int AddEnemy(const std::shared_ptr<Jam::Domain::Enemy::EnemyBase>& enemy,
					 const s3d::FilePath& animJsonPath);

		/// @brief 全敵の状態を更新する
		/// @param deltaTime フレーム間の経過時間
		void update(double deltaTime);

		/// @brief 全敵を描画する
		void draw();

		/// @brief 指定IDのAnimatorを取得
		/// @param id 敵ID
		/// Animatorへの参照
		Animator& getAnimator(int id);

	private:
		int m_nextID = 0;
		std::unordered_map<int, std::shared_ptr<Jam::Domain::Enemy::EnemyBase>> m_enemies;
		std::unordered_map<int, Animator> m_animators;
	};
}
