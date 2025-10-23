#pragma once
#pragma once
#include <Siv3D.hpp>

class RectSlide {
	Size area; //シーンの大きさ
	Array<Rect> rects;	//長方形たち
	Array<ColorF> colors;

public:
	RectSlide(Size s, int32 kazu = 30) :area(s)
	{
		init(kazu);
	};
	//ランダムに色を設定する (ピンクと紫系)
	ColorF randomcol() const {
		// 色相 (Hue): 270 (紫) ～ 330 (ピンク/マゼンタ) の範囲でランダムに選ぶ
		double hue = Random(270.0, 330.0);

		// 彩度 (Saturation): 0.5 ～ 1.0 (色の鮮やかさ)
		double saturation = Random(0.5, 1.0);

		// 明度 (Value): 0.7 ～ 1.0 (色の明るさ)
		// (暗すぎると黒っぽくなるため、下限を 0.7 程度に設定)
		double value = Random(0.7, 1.0);

		// HSVで指定した色を ColorF に変換して返す
		return ColorF{ HSV{ hue, saturation, value } };
	}

	//長方形の形、色、数を初期化
	void init(int kazu) {
		rects.clear();
		colors.clear();
		Array<int32> p;
		p << area.y;
		for (auto i : step(kazu)) {
			int32 num = p[0] * Random(0.2, 0.8);
			p << num << p[0] - num;
			p.remove_at(0);
		}
		int32 posy = 0;
		for (auto n : p) {
			rects << Rect{ -area.x, posy, area.x, n };
			posy += n;
			colors << randomcol();
		}
		rects.shuffle();
	}
	void drawFadeOut(double t) const {
		for (const auto& [i, rect] : Indexed(rects)) {
			const double nt = Clamp((t - (double)i * 0.6 / rects.size()) * 2.5, 0.0, 1.0);
			rect.movedBy(EaseInExpo(nt) * area.x, 0).draw(colors[i]);
		}
	}
	void drawFadeIn(double t) const {
		for (const auto& [i, rect] : Indexed(rects)) {
			const double nt = Clamp((t - (double)i * 0.6 / rects.size()) * 2.5, 0.0, 1.0);
			rect.movedBy(area.x + EaseOutExpo(nt) * area.x, 0).draw(colors[i]);
		}
	}
};

// RectSlideエフェクトをシーン間で共有するための管理者
class TransitionManager
{
public:
	// どこからでも唯一のインスタンスにアクセスできる
	static TransitionManager& Instance()
	{
		static TransitionManager instance;
		return instance;
	}
	
	// 共有するRectSlideオブジェクト
	RectSlide rec = RectSlide{ Scene::Size() };

private:
	// シングルトンのための設定
	TransitionManager() = default;
	~TransitionManager() = default;
	TransitionManager(const TransitionManager&) = delete;
	TransitionManager& operator=(const TransitionManager&) = delete;
};
