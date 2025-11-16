#pragma once
#include <Siv3D.hpp>

namespace Jam::Presentation::Scenes
{
	//シーン遷移時のアニメーションを管理するクラス
	class RectSlide {
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

	public:
		RectSlide(Size s, int32 amount = 30) : area(s)
		{
			init(amount);
		};

		//ランダムに色を設定する (ピンクと紫系)
		ColorF randomcol() const {
			// 色相 (Hue): 270 (紫) ～ 330 (ピンク/マゼンタ) の範囲でランダムに選ぶ
			double hue = Random(270.0, 330.0);
			// 彩度 (Saturation): 0.5 ～ 1.0 (色の鮮やかさ)
			double saturation = Random(0.5, 1.0);
			// 明度 (Value): 0.7 ～ 1.0 (色の明るさ)
			double value = Random(0.7, 1.0);
			// HSVで指定した色を ColorF に変換して返す
			return ColorF{ HSV{ hue, saturation, value } };
		}

		//長方形の形、色、数を初期化
		void init(int32 amount) {
			rects.clear();
			colors.clear();

			// 高さを分割していくためのリスト
			Array<int32> heightPartitions;
			heightPartitions << area.y; // まず全体の高さを追加

			// amount 回、リスト内の要素を分割する
			for (int32 i = 0; i < amount; ++i) {
				// リストの先頭の要素を取り出して分割する
				int32 currentHeight = heightPartitions[0];
				heightPartitions.remove_at(0);

				int32 num = static_cast<int32>(currentHeight * Random(MinSplitRatio, MaxSplitRatio));
				heightPartitions << num << (currentHeight - num); // 分割した2つをリストの末尾に追加
			}

			// 分割された高さのリストを元に長方形を生成
			int32 posy = 0;
			for (auto height : heightPartitions) {
				// 初期位置は画面左外側
				rects << Rect{ -area.x, posy, area.x, height };
				posy += height;
				colors << randomcol();
			}

			// 長方形の描画順をシャッフルすることで、アニメーションの順番をランダムにする
			rects.shuffle();
		}

		//フェードアウト（左から右に画面を覆う）アニメーションを描画
		void drawFadeOut(double t) const {
			for (const auto& [i, rect] : Indexed(rects)) {
				// 各長方形のアニメーション進行度 (インデックスiに応じて遅延させる)
				const double nt = Clamp((t - (double)i * StaggerDuration / rects.size()) * AnimationSpeed, 0.0, 1.0);
				// 画面左外 -> 画面上 にスライドイン
				rect.movedBy(EaseInExpo(nt) * area.x, 0).draw(colors[i]);
			}
		}

		//フェードイン（画面を覆っていたものが消える）アニメーションを描画
		void drawFadeIn(double t) const {
			for (const auto& [i, rect] : Indexed(rects)) {
				const double nt = Clamp((t - (double)i * StaggerDuration / rects.size()) * AnimationSpeed, 0.0, 1.0);
				// 画面上 -> 画面右外 にスライドアウト
				// (元のrect.xは -area.x なので、 area.x を足して 0 (画面左端) からスタート)
				rect.movedBy(area.x + EaseOutExpo(nt) * area.x, 0).draw(colors[i]);
			}
		}
	};



	//エフェクトをシーン間で共有するための管理者 (シングルトン)
	class TransitionManager
	{
	public:
		// 唯一のインスタンスを取得
		static TransitionManager& Instance()
		{
			static TransitionManager instance;
			return instance;
		}
		RectSlide rec;

		// --- publicな操作メソッド ---

		//共有エフェクトの FADE OUT 描画
		void drawFadeOut(double t) const
		{
			rec.drawFadeOut(t);
		}

		//共有エフェクトの FADE IN 描画
		void drawFadeIn(double t) const
		{
			rec.drawFadeIn(t);
		}

		//ランダムにするためにエフェクトの形状や色をリセット（再初期化）

		void reset(int32 amount = 30)
		{
			rec.init(amount);
		}

	private:


		TransitionManager()
			: rec(Scene::Size())
		{
		}
		~TransitionManager() = default;
		TransitionManager(const TransitionManager&) = delete;
		TransitionManager& operator=(const TransitionManager&) = delete;
	};
}
