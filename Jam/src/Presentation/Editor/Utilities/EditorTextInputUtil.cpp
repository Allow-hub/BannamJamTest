#include "EditorTextInputUtil.h"

namespace Jam::Presentation::Editor
{
	int EditorTextInputUtil::drawDoubleSlider(
		const String& label,
		double& value,
		double minValue,
		double maxValue,
		const Vec2& position,
		TextEditState& textEdit,
		int decimalPlaces,
		int labelWidth,
		int sliderWidth,
		int textBoxWidth
	)
	{
		int y = static_cast<int>(position.y);

		// スライダーを描画
		const String sliderLabel = formatSliderLabel(label, value, decimalPlaces);
		if (SimpleGUI::Slider(sliderLabel, value, minValue, maxValue, position, labelWidth, sliderWidth))
		{
			// スライダーが変更されたらテキストボックスも更新
			textEdit.text = Format(value, decimalPlaces);
		}
		y += SLIDER_HEIGHT;

		// テキストボックスの値を同期
		syncTextBoxValue(textEdit, value, decimalPlaces);

		// テキストボックスを描画
		const Vec2 textBoxPos{ position.x, static_cast<double>(y) };
		const RectF textBoxRect{ textBoxPos, textBoxWidth, 36 };

		if (SimpleGUI::TextBox(textEdit, textBoxPos, textBoxWidth))
		{
			// テキストが変更されたら値を更新
			if (auto parsedValue = ParseOpt<double>(textEdit.text))
			{
				value = Clamp(*parsedValue, minValue, maxValue);
			}
		}

		// フォーカス管理
		handleTextBoxFocus(textEdit, textBoxRect);

		y += TEXTBOX_HEIGHT;
		return y;
	}

	int EditorTextInputUtil::drawIntSlider(
		const String& label,
		int& value,
		int minValue,
		int maxValue,
		const Vec2& position,
		TextEditState& textEdit,
		int labelWidth,
		int sliderWidth,
		int textBoxWidth
	)
	{
		int y = static_cast<int>(position.y);

		// スライダーを描画（整数として）
		double doubleValue = static_cast<double>(value);
		const String sliderLabel = formatSliderLabel(label, doubleValue, 0);

		if (SimpleGUI::Slider(sliderLabel, doubleValue, static_cast<double>(minValue), static_cast<double>(maxValue), position, labelWidth, sliderWidth))
		{
			value = static_cast<int>(doubleValue);
			textEdit.text = Format(value);
		}
		y += SLIDER_HEIGHT;

		// テキストボックスの値を同期
		syncTextBoxValue(textEdit, static_cast<double>(value), 0);

		// テキストボックスを描画
		const Vec2 textBoxPos{ position.x, static_cast<double>(y) };
		const RectF textBoxRect{ textBoxPos, textBoxWidth, 36 };

		if (SimpleGUI::TextBox(textEdit, textBoxPos, textBoxWidth))
		{
			// テキストが変更されたら値を更新
			if (auto parsedValue = ParseOpt<int>(textEdit.text))
			{
				value = Clamp(*parsedValue, minValue, maxValue);
			}
		}

		// フォーカス管理
		handleTextBoxFocus(textEdit, textBoxRect);

		y += TEXTBOX_HEIGHT;
		return y;
	}

	void EditorTextInputUtil::handleTextBoxFocus(TextEditState& textEdit, const RectF& textBoxRect)
	{
		if (textEdit.active)
		{
			// Enter または Escape でフォーカスを解除
			if (KeyEnter.down() || KeyEscape.down())
				textEdit.active = false;
			// テキストボックスの外をクリックしたらフォーカスを解除
			else if (MouseL.down())
			{
				if (!textBoxRect.contains(Cursor::Pos()))
					textEdit.active = false;
			}
		}
	}

	String EditorTextInputUtil::formatSliderLabel(const String& label, double value, int decimalPlaces)
	{
		if (decimalPlaces == 0)
		{
			// 整数表示
			return U"{}: {}"_fmt(label, static_cast<int>(value));
		}
		else
		{
			// 小数表示
			return U"{}: {:.{}f}"_fmt(label, value, decimalPlaces);
		}
	}

	void EditorTextInputUtil::syncTextBoxValue(TextEditState& textEdit, double value, int decimalPlaces)
	{
		// テキストボックスがアクティブでない、または空の場合のみ更新
		if (textEdit.text.isEmpty() || !textEdit.active)
		{
			if (decimalPlaces == 0)
			{
				textEdit.text = Format(static_cast<int>(value));
			}
			else
			{
				textEdit.text = Format(value, decimalPlaces);
			}
		}
	}
}
