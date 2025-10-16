#pragma once
#include "IPlayerSkill.h"
#include "BombEntity.h"

namespace Jam::Domain::Player
{
	//爆弾スキル、押すと投げる
	class BombSkill : public IPlayerSkill
	{
	private:
		std::list<std::unique_ptr<BombEntity>> m_bombs;
		const size_t m_maxBombs = 3;

	public:
		void use(s3d::Vec2 position, bool facingRight) override
		{
			// 制限チェック
			if (m_bombs.size() >= m_maxBombs)
			{
				Print << U"❌ Bomb limit reached!";
				return;
			}

			// 爆弾生成（物理ボディ付き）
			Vec2 launchVelocity = facingRight ? Vec2{ 300, -200 } : Vec2{ -300, -200 };

			//auto body = PhysicsBodyFactory::CreateDynamicBody(position, launchVelocity);
			//auto bomb = std::make_unique<BombEntity>(std::move(body));

			m_bombs.push_back(std::move(bomb));

			Print(U"💣 Bomb thrown from ({}, {})", position.x, position.y);
		}

		void update(double deltaTime) override
		{
			for (auto it = m_bombs.begin(); it != m_bombs.end();)
			{
				(*it)->update(deltaTime);

				if (!(*it)->isAlive())
				{
					it = m_bombs.erase(it); // 爆発済みの爆弾を削除
				}
				else
				{
					++it;
				}
			}
		}

		bool needUpdate() override
		{
			return !m_bombs.empty();
		}
	};
}
