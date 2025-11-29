#pragma once
#include <Siv3D.hpp>
#include "ITransitionable.h"

namespace Jam::Presentation::Scenes
{
	// 円形に開閉するアイリストランジション
	class IrisTransition : public ITransitionable
	{
	public:
		// デフォルトは黒色
		IrisTransition(Size size, ColorF color = Palette::Black);

		void refresh() override;

		// フェードアウト（アイリスアウト：円が閉じて画面を隠す）
		void drawFadeOut(double t) const override;

		// フェードイン（アイリスイン：円が広がって画面を見せる）
		void drawFadeIn(double t) const override;

		// アイリスの中心点を変更したい場合に使用
		void setCenter(const Vec2& center);

		// マスクの色を変更したい場合に使用
		void setColor(const ColorF& color);

	private:
		Size m_area;        // 画面サイズ
		ColorF m_color;     // マスクの色
		Vec2 m_center;      // 円の中心
		double m_maxRadius; // 画面全体を覆うために必要な最大半径
	};
}
