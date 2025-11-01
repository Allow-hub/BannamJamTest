#pragma once
#include "IIndependentObject.h"
#include "Physics/IPhysicsBody.h"
#include "Physics/ICollisionListener.h"
#include "../Presentation/AudioService.h"
#include "../Foundation/CoreManager.h"

namespace Jam::Domain
{
	class GoalArea :public Jam::Domain::IIndependentObject, public Jam::Domain::Physics::ICollisionListener
	{
	private:
		bool m_collected = false;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		std::function<void()> m_onClear;
		Texture m_texture;
		double m_scale = 0.3;
		Vec2 m_offset = Vec2(100, 100);

	public:
		GoalArea(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, std::function<void()> onClear)
			: m_body(body), m_onClear(onClear)
		{
			m_texture = Texture(U"Assets/Stage/goal.png");
		}

		void update(double deltaTime)override
		{
		}

		void draw() const override
		{
			m_texture.scaled(m_scale).draw(m_body->getPosition() - m_offset);

			// （デバッグ用）
			//m_body->drawFrame(2.0, Palette::Yellow);
		}

		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override
		{
			switch (other->getLayer())
			{
			case Jam::Domain::Physics::PhysicsLayer::Player:
				if (m_collected) return;
				// ゴール時のSEを再生
				Jam::Presentation::AudioService::get().playOneShot(
					Jam::Presentation::AudioService::Sound::SE_Goal,
					1.0
				);
				//クリアフラグを立てる
				Jam::Foundation::CoreManager::Instance().setClear(true);
				if (m_onClear) m_onClear();
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
