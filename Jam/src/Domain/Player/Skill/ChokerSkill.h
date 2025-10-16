#pragma once
#include "IPlayerSkill.h"

namespace Jam::Domain::Player
{
	//チョーカースキル、グラップル的な
	class ChokerSkill : public IPlayerSkill
	{
	public:
		void use(s3d::Vec2 position, bool facingRight) override
		{
			// グラップル処理
			Print(U"Grapple used facing {}");
		}

		void update(double deltaTime) override
		{
			// フックの飛行や引き寄せ判定
		}

		bool needUpdate() override
		{
			return	false;
		}
	};
}
