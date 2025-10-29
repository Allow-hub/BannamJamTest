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

	public:
		GoalArea(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, std::function<void()> onClear)
			: m_body(body), m_onClear(onClear)
		{
		}

		void update(double deltaTime)override
		{
		}

		void draw() const override
		{
			m_body->drawFrame(2.0, m_collected ? Palette::Black : Palette::Yellow);
		}

		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override
		{
			switch (other->getLayer())
			{
			case Jam::Domain::Physics::PhysicsLayer::Player:
				if (m_collected) return;
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
