#pragma once
#include <Siv3D.hpp>

namespace Jam::Infrastructure
{
	class CursorUtil
	{
	public:
		// シングルトン取得
		static CursorUtil& instance()
		{
			static CursorUtil inst;
			return inst;
		}
		// カーソルを切り替え
		void setCursor(CursorStyle style)
		{
			Cursor::RequestStyle(style);
		}

		void setClipWindowCuror(bool b)
		{
			Cursor::ClipToWindow(b);
		}

		//カーソルのポイントを小数点付きで返す
		Vec2 getCursorPosF()
		{
			return Cursor::PosF();
		}
	private:
		CursorUtil() = default;
		~CursorUtil() = default;
	};
}
