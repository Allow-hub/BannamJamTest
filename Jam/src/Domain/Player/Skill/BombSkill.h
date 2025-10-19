#pragma once
#include "IPlayerSkill.h"
#include "BombEntity.h"
#include "../../../Infrastructure/Siv3DPhysicsBodyFactory.h"
#include "../../../Infrastructure/FactoryServiceLocator.h"


namespace Jam::Domain::Player
{
	//爆弾スキル、押すと投げる
	class BombSkill : public IPlayerSkill
	{
	private:
		bool m_pushing = false;
		std::list<std::shared_ptr<BombEntity>> m_bombs;
		const size_t m_maxBombs = 3;
		const Vec2 createOffset = Vec2{ 50,-30 };

	public:
		BombSkill(Jam::Domain::Events::GameEventQueue& eventQueue)
			: IPlayerSkill(PlayerSkillType::Bomb,eventQueue) {}

		void use(const s3d::Vec2 position, bool facingRight) override
		{
			if (m_pushing)return;
			// 制限チェック
			if (m_bombs.size() >= m_maxBombs)
			{
				//Print << U"❌ Bomb limit reached!";
				return;
			}

			// 投げる方向に応じてXだけ反転
			Vec2 offset = facingRight ? createOffset : Vec2{ -createOffset.x, createOffset.y };
			auto body = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()
				->createBody(
					position + offset,
					SizeF{ 40, 40 },                // 爆弾サイズ
					s3d::P2BodyType::Dynamic,
					Jam::Domain::Physics::PhysicsMaterial{ 0.5, 0.0, 0.8 ,1.0 },
					Jam::Domain::Physics::PhysicsShape::Circle
				);
			body->setLayer(Jam::Domain::Physics::PhysicsLayer::Weapon);
			auto bomb = std::make_shared<BombEntity>(body);
			body->setCollisionListener(
				std::static_pointer_cast<Jam::Domain::Physics::ICollisionListener>(bomb)
			);
			m_bombs.push_back(bomb);

			m_pushing = true;
		}

		void useReleased(const s3d::Vec2 position, bool facingRight) override
		{
			m_pushing = false;
		}

		void update(double deltaTime)  override
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

		void draw() const override
		{
			for (const auto& bomb : m_bombs)
			{
				// BombEntity 内の Body を描画
				bomb->getBody()->drawFrame(3.0, Palette::Darkcyan);
			}
		}


		bool needUpdate() const override
		{
			return !m_bombs.empty();
		}
	};
}
