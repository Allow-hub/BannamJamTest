#include <Siv3D.hpp>       
#include "RectSlide.h" 

namespace Jam::Presentation::Scenes
{
	// コンストラクタ
	RectSlide::RectSlide(Size s, int32 amount)
		: area(s) // メンバ初期化子リスト
	{
		init(amount); 
	}

	ColorF RectSlide::randomcol() const
	{
		double hue = Random(270.0, 330.0);
		double saturation = Random(0.5, 1.0);
		double value = Random(0.7, 1.0);
		return ColorF{ HSV{ hue, saturation, value } };
	}

	void RectSlide::init(int32 amount)
	{
		rects.clear();
		colors.clear();

		Array<int32> heightPartitions;
		heightPartitions << area.y;

		for (int32 i = 0; i < amount; ++i)
		{
			int32 currentHeight = heightPartitions[0];
			heightPartitions.remove_at(0);

			int32 num = static_cast<int32>(currentHeight * Random(MinSplitRatio, MaxSplitRatio));
			heightPartitions << num << (currentHeight - num);
		}

		int32 posy = 0;
		for (auto height : heightPartitions)
		{
			rects << Rect{ -area.x, posy, area.x, height };
			posy += height;
			colors << randomcol();
		}

		rects.shuffle();
	}

	// フェードアウト
	void RectSlide::drawFadeOut(double t) const
	{
		for (const auto& [i, rect] : Indexed(rects))
		{
			const double nt = Clamp((t - (double)i * StaggerDuration / rects.size()) * AnimationSpeed, 0.0, 1.0);
			rect.movedBy(EaseInExpo(nt) * area.x, 0).draw(colors[i]);
		}
	}

	// フェードイン
	void RectSlide::drawFadeIn(double t) const
	{
		for (const auto& [i, rect] : Indexed(rects))
		{
			const double nt = Clamp((t - (double)i * StaggerDuration / rects.size()) * AnimationSpeed, 0.0, 1.0);
			rect.movedBy(area.x + EaseOutExpo(nt) * area.x, 0).draw(colors[i]);
		}
	}
}
