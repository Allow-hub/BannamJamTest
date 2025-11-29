#pragma once
#include <Siv3D.hpp> 

namespace Jam::Presentation::Scenes
{
	enum class TransitionType
	{
		RectSlide,
		Iris,
	};

	//シーン遷移アニメーションのインターフェース（抽象クラス）
	class ITransitionable
	{
	public:
		virtual ~ITransitionable() = default;
		virtual void refresh() = 0;
		virtual void drawFadeOut(double t) const = 0;
		virtual void drawFadeIn(double t) const = 0;
	};

}
