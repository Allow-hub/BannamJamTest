#pragma once
#include <Siv3D.hpp>
#include "Presentation/CameraManager.h"

namespace Jam::Infrastructure
{
	class CursorUtil
	{
	private:
		std::weak_ptr<Jam::Presentation::CameraManager> m_cameraManager;

		CursorUtil() = default;
		~CursorUtil() = default;

		// コピー・ムーブを禁止
		CursorUtil(const CursorUtil&) = delete;
		CursorUtil& operator=(const CursorUtil&) = delete;
		CursorUtil(CursorUtil&&) = delete;
		CursorUtil& operator=(CursorUtil&&) = delete;

	public:
		// シングルトン取得
		static CursorUtil& instance()
		{
			static CursorUtil inst;
			return inst;
		}

		void setCameraManager(std::shared_ptr<Jam::Presentation::CameraManager> cameraManager)
		{
			m_cameraManager = cameraManager;
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

		// ゲーム内座標を取得（全変換を考慮）
		Vec2 getCursorPosF() const
		{
			if (auto camera = m_cameraManager.lock())
			{
				return camera->getCursorWorldPos();
			}
			// カメラが設定されていない場合はスクリーン座標をそのまま返す
			return Cursor::PosF();
		}


		/// @brief カスタムカーソルを登録
		/// @param name 登録名
		/// @param image カーソル画像
		/// @param hotSpot 画像中のクリック位置
		/// @return 登録に成功した場合 true
		bool registerCustomCursor(StringView name, const Image& image, Point hotSpot = Point{ 0, 0 })
		{
			return Cursor::RegisterCustomCursorStyle(name, image, hotSpot);
		}

		void requestStyle(StringView name)
		{
			return Cursor::RequestStyle(name);
		}
	};
}
