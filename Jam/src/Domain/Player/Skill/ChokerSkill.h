#pragma once
#include "IPlayerSkill.h"
#include "../../Physics/ICollisionListener.h"
#include "../../Events/GameEvents.h"
#include "../../../Infrastructure/Siv3DCursorUtil.h"

namespace Jam::Domain::Player
{
	//チョーカースキル、グラップル的な
	class ChokerSkill :public Jam::Domain::Physics::ICollisionListener, public IPlayerSkill, public std::enable_shared_from_this<ChokerSkill>
	{
	private:
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;

	public:
		ChokerSkill(Jam::Domain::Events::GameEventQueue& eventQueue)
			: IPlayerSkill(PlayerSkillType::Choker,eventQueue)
		{
			m_body = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()
				->createBody(
				Vec2{ 0, 0 },
				SizeF{ 50, 100 },
				s3d::P2BodyType::Dynamic,
				Jam::Domain::Physics::PhysicsMaterial{ 0.0,0.0,0.1 }
				);
			m_body->setCollisionListener(shared_from_this());
		}

		void use(const s3d::Vec2 position, bool facingRight) override
		{
			//Jam::Infrastructure::CursorUtil::instance().getCursorPosF()
		}

		void draw() const override
		{
			if (!m_body)return;
			m_body->drawFrame(3.0, Palette::Darkgoldenrod);
		}

		void useReleased(const s3d::Vec2 position, bool facingRight) override
		{
		}
		void update(double deltaTime) override
		{
			// フックの飛行や引き寄せ判定
		}

		bool needUpdate() const override
		{
			return	false;
		}

		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)override
		{
		}

		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)override {}
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)override {}
	};
}
