#pragma once
#include <Siv3D.hpp> 

namespace Jam::Presentation::Scenes
{
	/**
	 シーン遷移アニメーションのインターフェース（抽象クラス）
	 */
	class ITransition
	{
	public:
		virtual ~ITransition() = default;
		virtual void init(int32 amount) = 0;
		virtual void drawFadeOut(double t) const = 0;
		virtual void drawFadeIn(double t) const = 0;
	};
}
