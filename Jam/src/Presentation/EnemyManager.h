#pragma once
#include <Siv3D.hpp>
#include <unordered_map>
#include <memory>
#include "../Domain/Enemy/EnemyBase.h"
#include "../UseCase/EnemyFactory.h"
#include "Animator.h"
#include "AnimatorLoader.h"

namespace Jam::Presentation
{
	class EnemyManager
	{
	public:
		EnemyManager() : m_nextID(0) {}

		// Enemy と Animator を同時に登録
		int AddEnemy(const std::shared_ptr<Jam::Domain::Enemy::EnemyBase>& enemy,
					 const s3d::FilePath& animJsonPath)
		{
			int id = m_nextID++;
			m_enemies[id] = enemy;

			Animator animator;
			if (!AnimatorLoader::LoadAnimatorFromJSON(animator, animJsonPath))
			{
				Console << U"[EnemyManager] ⚠ Failed to load animator for enemy ID " << id;
			}
			m_animators[id] = animator;

			return id;
		}

		void RemoveEnemy(int id)
		{
			m_enemies.erase(id);
			m_animators.erase(id);
		}

		void update(double deltaTime)
		{
			for (auto& [id, enemy] : m_enemies)
			{
				if (!enemy->isAlive()) continue;

				m_animators[id].Update(deltaTime);
			}
		}

		void draw() const
		{
			for (auto& [id, enemy] : m_enemies)
			{
				if (!enemy->isAlive()) continue;
				auto pos = enemy->getPhysicsBody()->getPosition();
				m_animators.at(id).Draw(pos);
			}
		}

		std::shared_ptr<Jam::Domain::Enemy::EnemyBase> GetEnemy(int id)
		{
			auto it = m_enemies.find(id);
			if (it != m_enemies.end()) return it->second;
			return nullptr;
		}

		Animator& GetAnimator(int id)
		{
			return m_animators.at(id);
		}

	private:
		int m_nextID;
		std::unordered_map<int, std::shared_ptr<Jam::Domain::Enemy::EnemyBase>> m_enemies;
		std::unordered_map<int, Animator> m_animators;
	};
}
