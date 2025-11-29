#include "RectSlide.h" 

namespace Jam::Presentation::Scenes
{
	RectSlide::RectSlide(Size size, int32 amount)
		: m_area(size)
		, m_splitAmount(amount)
	{
		m_rects.reserve(m_splitAmount * 2);
		m_colors.reserve(m_splitAmount * 2);

		// 初回の生成
		refresh();
	}

	ColorF RectSlide::generateRandomColor() const
	{
		double hue = Random(270.0, 330.0);
		double saturation = Random(0.5, 1.0);
		double value = Random(0.7, 1.0);
		return ColorF{ HSV{ hue, saturation, value } };
	}

	void RectSlide::refresh()
	{
		m_rects.clear();
		m_colors.clear();

		// 分割ロジック
		Array<int32> heightPartitions;
		heightPartitions << m_area.y;

		// 指定された回数だけ分割
		for (int32 i = 0; i < m_splitAmount; ++i)
		{
			if (heightPartitions.isEmpty()) break;

			int32 currentHeight = heightPartitions[0];
			heightPartitions.remove_at(0);

			// 分割しすぎないように
			if (currentHeight <= 1)
			{
				heightPartitions << currentHeight;
				continue;
			}

			int32 num = static_cast<int32>(currentHeight * Random(MinSplitRatio, MaxSplitRatio));
			// 最低1ピクセルは確保
			num = Max(1, num);

			heightPartitions << num << (currentHeight - num);
		}

		// RectとColorの生成
		int32 posy = 0;
		for (const auto& height : heightPartitions)
		{
			m_rects << Rect{ -m_area.x, posy, m_area.x, height };
			m_colors << generateRandomColor();
			posy += height;
		}

		// 順番をシャッフルすることで毎回違う動きに見せる
		m_rects.shuffle();
	}

	// フェードアウト
	void RectSlide::drawFadeOut(double t) const
	{
		if (m_rects.isEmpty()) return;

		const size_t count = m_rects.size();
		for (size_t i = 0; i < count; ++i)
		{
			const double progress = Clamp((t - (double)i * StaggerDuration / count) * AnimationSpeed, 0.0, 1.0);

			m_rects[i].movedBy(EaseInExpo(progress) * m_area.x, 0).draw(m_colors[i]);
		}
	}

	// フェードイン
	void RectSlide::drawFadeIn(double t) const
	{
		if (m_rects.isEmpty()) return;

		const size_t count = m_rects.size();
		for (size_t i = 0; i < count; ++i)
		{
			const double progress = Clamp((t - (double)i * StaggerDuration / count) * AnimationSpeed, 0.0, 1.0);
			
			// 画面を覆った状態からさらに右へ抜ける
			m_rects[i].movedBy(m_area.x + EaseOutExpo(progress) * m_area.x, 0).draw(m_colors[i]);
		}
	}
}
