#include "IrisTransition.h"

namespace Jam::Presentation::Scenes
{
	IrisTransition::IrisTransition(Size size, ColorF color)
		: m_area(size)
		, m_color(color)
	{
		refresh();
	}

	void IrisTransition::refresh()
	{
		// デフォルトでは画面の中心をアイリスの中心にする
		m_center = m_area / 2.0;

		// 画面の隅まで覆うため、中心から四隅までの最大距離を計算する
		m_maxRadius = m_center.length() * 1.5;
	}

	void IrisTransition::drawFadeOut(double t) const
	{
		// t: 0.0 -> 1.0
		// 半径: 最大 -> 0 (閉じる)
		const double tEase = EaseInOutExpo(t);
		const double currentRadius = Math::Lerp(m_maxRadius, 0.0, tEase);

		// drawFrame(innerThickness, outerThickness, color)
		Circle{ m_center, currentRadius }
		.drawFrame(0.0, m_maxRadius, m_color);
	}

	void IrisTransition::drawFadeIn(double t) const
	{
		// t: 0.0 -> 1.0
		// 半径: 0 -> 最大 (開く)
		const double tEase = EaseInOutExpo(t);
		const double currentRadius = Math::Lerp(0.0, m_maxRadius, tEase);

		// 中心から広がる円の外側を塗りつぶす
		Circle{ m_center, currentRadius }
		.drawFrame(0.0, m_maxRadius, m_color);
	}

	void IrisTransition::setCenter(const Vec2& center)
	{
		m_center = center;
		// 中心が変わった場合、最大半径が変わる可能性があるため再計算
		double distTL = m_center.distanceFrom(Vec2{ 0, 0 });
		double distTR = m_center.distanceFrom(Vec2{ m_area.x, 0 });
		double distBL = m_center.distanceFrom(Vec2{ 0, m_area.y });
		double distBR = m_center.distanceFrom(Vec2{ m_area.x, m_area.y });

		m_maxRadius = Max({ distTL, distTR, distBL, distBR });
	}

	void IrisTransition::setColor(const ColorF& color)
	{
		m_color = color;
	}
}
