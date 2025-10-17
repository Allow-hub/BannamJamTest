#pragma once
#include "IPlayerSkill.h"

namespace Jam::Domain::Player
{
	//チョーカースキル、グラップル的な
	class ChokerSkill : public IPlayerSkill
	{
	public:
		ChokerSkill()
			: IPlayerSkill(PlayerSkillType::Choker) {}

		void use(const s3d::Vec2 position, bool facingRight) override
		{
			// グラップル処理
			Print(U"Grapple used facing {}");
		}

		void draw() const override
		{
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
	};
}
