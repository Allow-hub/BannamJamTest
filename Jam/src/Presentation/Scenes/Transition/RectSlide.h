#pragma once
#include "ITransition.h" // 親クラスの定義をインクルード

namespace Jam::Presentation::Scenes
{
	/**
	 * @brief 四角形がスライドするトランジション (宣言)
	 *
	 * ITransition インターフェースを継承することを明記します。
	 */
	class RectSlide : public ITransition // <-- ★★★ ここの継承指定が重要 ★★★
	{
	private:
		// init()での高さ分割の最小・最大比率
		static constexpr double MinSplitRatio = 0.2;
		static constexpr double MaxSplitRatio = 0.8;

		// draw()でのアニメーションの遅延（ズレ）の幅
		static constexpr double StaggerDuration = 0.6;

		// draw()での各長方形のアニメーション速度
		static constexpr double AnimationSpeed = 2.5;

		Size area; //シーンの大きさ
		Array<Rect> rects;	//長方形たち
		Array<ColorF> colors; // 長方形の色

		/**
		 * @brief ランダムに色を設定します (ピンクと紫系)
		 * @return ColorF ランダムな色
		 */
		ColorF randomcol() const; // 実装は .cpp へ

	public:
		/**
		 * @brief コンストラクタ
		 * @param s シーンのサイズ
		 * @param amount 初期化時の分割数
		 */
		RectSlide(Size s, int32 amount = 30); // 実装は .cpp へ

		/**
		 * @brief 長方形の形、色、数を初期化します。
		 * @param amount 分割数
		 */
		void init(int32 amount) override; // override と宣言 (実装は .cpp へ)

		/**
		 * @brief フェードアウト（左から右に画面を覆う）アニメーションを描画します。
		 * @param t 進行度 (0.0 ～ 1.0)
		 */
		void drawFadeOut(double t) const override; // override と宣言 (実装は .cpp へ)

		/**
		 * @brief フェードイン（画面を覆っていたものが消える）アニメーションを描画します。
		 * @param t 進行度 (0.0 ～ 1.0)
		 */
		void drawFadeIn(double t) const override; // override と宣言 (実装は .cpp へ)
	};
}
