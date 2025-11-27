#pragma once
#include <Siv3D.hpp>  
#include "ITransitionable.h"
namespace Jam::Presentation::Scenes
{
	//四角形がスライドするトランジション
	class RectSlide : public ITransitionable
	{
	public:
		// RectSlideの標準は30分割
		static constexpr int32 DefaultSplitAmount = 30;

		RectSlide(Size size, int32 amount = DefaultSplitAmount);

		void refresh() override;

		//フェードアウト（左から右に画面を覆う）アニメーションを描画
		void drawFadeOut(double t) const override;


		//フェードイン（画面を覆っていたものが消える）アニメーションを描画
		void drawFadeIn(double t) const override;

	private:
		// init()での高さ分割の最小・最大比率
		static constexpr double MinSplitRatio = 0.2;
		static constexpr double MaxSplitRatio = 0.8;

		// draw()でのアニメーションの遅延（ズレ）の幅
		static constexpr double StaggerDuration = 0.6;

		// draw()での各長方形のアニメーション速度
		static constexpr double AnimationSpeed = 2.5;

		Size m_area;         // 画面サイズ
		int32 m_splitAmount; // 分割数（設定値）

		Array<Rect> m_rects;    // 生成された長方形
		Array<ColorF> m_colors; // 生成された色

		//ランダムに色を設定
		ColorF generateRandomColor() const;
	};
}
