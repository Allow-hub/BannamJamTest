#pragma once
#include "Domain/Physics/IPhysicsBody.h"
#include "Domain/Physics/ICollisionListener.h"
#include "Infrastructure/FactoryServiceLocator.h"
#include "Infrastructure/IPhysicsBodyFactory.h"
#include "Presentation/AudioService.h"

namespace Jam::Domain
{
	class FlagmentMemory : public Jam::Domain::Physics::ICollisionListener
	{
	private:
		Vec2 m_position;
		bool m_collected = false;
		Texture m_texture;
		double m_angle = 0.0;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;

	public:
		FlagmentMemory(const Vec2& pos, std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body)
			: m_position(pos)
			, m_body(body)
			, m_texture(Resource(U"Assets/FlagmentMemory.png"))
		{
		}

		void update(double deltaTime)
		{
			if (m_collected) return;

			m_angle += deltaTime * 360_deg * 0.3;
			if (m_angle >= 360_deg)
				m_angle -= 360_deg;
		}

		void draw() const
		{
			if (m_collected) return;

			double scaleX = Math::Cos(m_angle);
			double absScale = Math::Abs(scaleX);
			const double baseSize = 150;

			m_texture
				.resized(baseSize)
				.scaled(absScale, 1.0)
				.drawAt(m_position);
		}

		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override
		{
			if (m_collected) return;

			switch (other->getLayer())
			{
			case Jam::Domain::Physics::PhysicsLayer::Player:
				Jam::Presentation::AudioService::get().playOneShot(Jam::Presentation::AudioService::Sound::SE_GetFlagment, 0.1);
				// フラグメント取得
				Jam::Foundation::CoreManager::Instance().addFlagment(1);

				// 自身の物理ボディを削除
				Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
					.getPhysicsFactory()->removeBody(m_body->getID());

				// フラグを立てて描画停止
				m_collected = true;

				break;
			default:
				break;
			}
		}

		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override {}
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override {}
	};
}
