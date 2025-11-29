#pragma once
#include <Siv3D.hpp>

namespace Jam::Presentation::Editor
{
	// エディタ用のテキスト入力ユーティリティクラス
	// スライダー + テキストボックスの組み合わせを提供
	class EditorTextInputUtil
	{
	public:
		// 浮動小数点数用のスライダー + テキストボックス
		// @return 次のY座標
		static int drawDoubleSlider(
			const String& label,
			double& value,
			double minValue,
			double maxValue,
			const Vec2& position,
			TextEditState& textEdit,
			int decimalPlaces = 1,
			int labelWidth = 100,
			int sliderWidth = 160,
			int textBoxWidth = 120
		);

		// 整数用のスライダー + テキストボックス
		// @return 次のY座標
		static int drawIntSlider(
			const String& label,
			int& value,
			int minValue,
			int maxValue,
			const Vec2& position,
			TextEditState& textEdit,
			int labelWidth = 100,
			int sliderWidth = 160,
			int textBoxWidth = 120
		);

	private:
		static void handleTextBoxFocus(TextEditState& textEdit, const RectF& textBoxRect);
		static String formatSliderLabel(const String& label, double value, int decimalPlaces);
		static void syncTextBoxValue(TextEditState& textEdit, double value, int decimalPlaces);

		static constexpr int SLIDER_HEIGHT = 35;
		static constexpr int TEXTBOX_HEIGHT = 45;
		static constexpr int TOTAL_HEIGHT = SLIDER_HEIGHT + TEXTBOX_HEIGHT;
	};
}
