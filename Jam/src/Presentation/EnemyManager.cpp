#include "EnemyManager.h"
#include "AnimatorLoader.h"
#include "../Domain/Physics/IPhysicsBody.h"

namespace Jam::Presentation
{
	int EnemyManager::AddEnemy(const std::shared_ptr<Jam::Domain::Enemy::EnemyBase>& enemy,
							   const s3d::FilePath& animJsonPath)
	{
		int id = m_nextID++;
		m_enemies[id] = enemy;

		Animator animator;
		if (!AnimatorLoader::loadAnimatorFromJSON(animator, animJsonPath))
		{
			// Print << U"[EnemyManager] ⚠ Failed to load animator for enemy ID " << id;
		}
		m_animators[id] = animator;

		// Enemy → Animator イベント接続
		enemy->setOnAnimationChange([this, id](const s3d::String& animName) {
			auto it = m_animators.find(id);
			if (it != m_animators.end())
			{
				it->second.setBoolExclusive(animName);
			}
		});

		return id;
	}

	void EnemyManager::update(double deltaTime)
	{
		for (auto& [id, enemy] : m_enemies)
		{
			if (!enemy->isAlive()) continue;
			enemy->update(deltaTime);

			auto it = m_animators.find(id);
			if (it != m_animators.end())
			{
				it->second.update(deltaTime);
			}
		}
	}

	void EnemyManager::draw()
	{
		for (auto& [id, enemy] : m_enemies)
		{
			if (!enemy->isAlive()) continue;

			enemy->draw();

			auto body = enemy->getPhysicsBody();
			if (!body) continue;

			auto pos = body->getPosition();
			auto it = m_animators.find(id);

			if (it != m_animators.end())
			{
				it->second.setFacingLeft(enemy->isFaceLeft());
				it->second.draw(pos);
			}
			else
			{
				Console << U"[EnemyManager] ⚠ No animator found for enemy " << id;
				Circle(pos, 25).draw(Palette::Red);
			}
		}
	}

	Animator& EnemyManager::getAnimator(int id)
	{
		return m_animators.at(id);
	}
}
