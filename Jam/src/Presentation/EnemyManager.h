#pragma once
#include <Siv3D.hpp>
#include <unordered_map>
#include "../Domain/Enemy/EnemyBase.h"
#include "Animator.h"
#include "AnimatorLoader.h"

namespace Jam::Presentation
{
	class EnemyManager
	{
	public:
		int AddEnemy(const std::shared_ptr<Jam::Domain::Enemy::EnemyBase>& enemy,
					 const s3d::FilePath& animJsonPath)
		{
			int id = m_nextID++;
			m_enemies[id] = enemy;

			Animator animator;
			if (!AnimatorLoader::LoadAnimatorFromJSON(animator, animJsonPath))
			{
				//Print << U"[EnemyManager] ⚠ Failed to load animator for enemy ID " << id;
			}
			m_animators[id] = animator;

			// :small_blue_diamond: Enemy → Animator イベント接続
			enemy->setOnAnimationChange([this, id](const s3d::String& animName) {
				auto it = m_animators.find(id);
				if (it != m_animators.end())
				{
					it->second.SetBoolExclusive(animName);
				}
			});
			return id;
		}

		void update(double deltaTime)
		{
			for (auto& [id, enemy] : m_enemies)
			{
				if (!enemy->isAlive()) continue;
				enemy->update(deltaTime);

				auto it = m_animators.find(id);
				if (it != m_animators.end())
				{
					it->second.Update(deltaTime);
				}
			}
		}

		void draw()
		{
			for (auto& [id, enemy] : m_enemies)
			{
				if (!enemy->isAlive())
				{
					continue;
				}
				//Animationはイベントを送る形をとるが敵のアイテム系はそれぞれが描画
				enemy->draw();//単体オブジェクトは他に描画を任せたが移植が住んでない→Eyeのビーム
				auto body = enemy->getPhysicsBody();
				//body->drawFrame(2.0, Palette::Red);
				if (!body)
				{
					continue;
				}

				auto pos = body->getPosition();

				auto it = m_animators.find(id);
				if (it != m_animators.end())
				{
					it->second.SetFacingLeft(enemy->isFaceLeft());
					it->second.Draw(pos);
				}
				else
				{
					Console << U"[EnemyManager] ⚠ No animator found for enemy " << id;
					// フォールバック: 赤い円を描画
					Circle(pos, 25).draw(Palette::Red);
				}
			}
		}

		Animator& getAnimator(int id) {
			return m_animators.at(id);
		}

	private:
		int m_nextID = 0;
		std::unordered_map<int, std::shared_ptr<Jam::Domain::Enemy::EnemyBase>> m_enemies;
		std::unordered_map<int, Animator> m_animators;
	};
}
