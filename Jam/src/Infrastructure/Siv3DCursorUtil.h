#pragma once
#include <Siv3D.hpp>
#include "../Presentation/CameraManager.h"

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
	};
}
