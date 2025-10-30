#pragma once
#include <HamFramework.hpp>
namespace Jam::Presentation
{
	enum class CameraMode
	{
		FollowPlayer,
		FocusPoint,
		Cinematic
	};
	class CameraManager
	{
	private:
		Camera2D m_camera;
		Vec2 m_target;
		double m_zoom = 1.0;
		double m_targetZoom = 1.0;
		CameraMode m_mode = CameraMode::FollowPlayer;
		Vec2 m_focusPoint;
		double m_modeDuration = 0.0;
		double m_shakeIntensity = 0.0;
		double m_shakeDuration = 0.0;
		double m_zoomDuration = 0.0;
		double m_totalZoomDuration = 0.0;
		bool m_isZoomLocked = false; // ズームをロックするフラグ
		double m_lockedBaseZoom = 1.0; // ロック時の基準ズーム値

		// Y座標の制限
		double m_minY = -std::numeric_limits<double>::infinity();
		double m_maxY = std::numeric_limits<double>::infinity();

	public:
		CameraManager(const Vec2& initPos = { 0, 0 })
		{
			m_camera = Camera2D(initPos, 1.0, CameraControl::None_);
			m_target = initPos;
		}

		// Y座標の範囲を設定
		void setYLimits(double minY, double maxY)
		{
			m_minY = minY;
			m_maxY = maxY;
		}

		// Y座標の最小値を設定
		void setMinY(double minY)
		{
			m_minY = minY;
		}

		// Y座標の最大値を設定
		void setMaxY(double maxY)
		{
			m_maxY = maxY;
		}

		// Y座標の制限をリセット
		void resetYLimits()
		{
			m_minY = -std::numeric_limits<double>::infinity();
			m_maxY = std::numeric_limits<double>::infinity();
		}

		void setTarget(const Vec2& target)
		{
			m_target = target;
		}

		void setZoom(double zoom, double zoomDuration)
		{
			// ロック中の場合、ロックされた基準ズームを基準として新しいズームを設定
			if (m_isZoomLocked)
			{
				m_targetZoom = Clamp(zoom * m_lockedBaseZoom, 0.01, 2.0);
			}
			else
			{
				m_targetZoom = Clamp(zoom, 0.01, 2.0);
			}
			m_zoomDuration = zoomDuration;
			m_totalZoomDuration = zoomDuration;
			// ロックは維持（解除しない）
		}

		void focusOn(const Vec2& point, double duration, double zoom = 1.0)
		{
			m_mode = CameraMode::FocusPoint;
			m_focusPoint = point;
			m_modeDuration = duration;
			// ロック中の場合、ロックされた基準ズームを基準として新しいズームを設定
			if (m_isZoomLocked)
			{
				m_targetZoom = zoom * m_lockedBaseZoom;
			}
			else
			{
				m_targetZoom = zoom;
			}
			// ロックは維持（解除しない）
		}

		void shake(double intensity, double duration)
		{
			m_shakeIntensity = intensity;
			m_shakeDuration = duration;
		}

		void resetToFollow()
		{
			m_mode = CameraMode::FollowPlayer;
			m_modeDuration = 0.0;
			m_isZoomLocked = false; // フォローモードに戻るときはロック解除
		}

		// 永続的なフォーカスモード（duration を無限にする）
		void lockFocusOn(const Vec2& point, double zoom = 1.0)
		{
			m_mode = CameraMode::FocusPoint;
			m_focusPoint = point;
			m_modeDuration = std::numeric_limits<double>::infinity(); // 無限に継続
			m_targetZoom = Clamp(zoom, 0.01, 2.0);
			m_lockedBaseZoom = m_targetZoom; // 基準ズーム値を記録
			m_isZoomLocked = true; // ズームをロック
			// 即座にズームを適用
			m_camera.setScale(m_targetZoom);
			m_zoomDuration = 0.0;
		}

		// フォーカスポイントを更新（ロック中でも位置だけ変更可能）
		void updateFocusPoint(const Vec2& point)
		{
			if (m_mode == CameraMode::FocusPoint)
			{
				m_focusPoint = point;
			}
		}

		void update(double deltaTime)
		{
			// === モード更新 ===
			if (m_modeDuration > 0.0 && m_modeDuration != std::numeric_limits<double>::infinity())
			{
				m_modeDuration -= deltaTime;
				if (m_modeDuration <= 0.0)
				{
					resetToFollow();
				}
			}

			Vec2 actualTarget = m_target;
			switch (m_mode)
			{
			case CameraMode::FollowPlayer:
				actualTarget = m_target;
				break;
			case CameraMode::FocusPoint:
				actualTarget = m_focusPoint;
				break;
			case CameraMode::Cinematic:
				break;
			}

			// === Y座標の制限を適用 ===
			actualTarget.y = Clamp(actualTarget.y, m_minY, m_maxY);

			// === カメラシェイク ===
			Vec2 shakeOffset = Vec2::Zero();
			if (m_shakeDuration > 0.0)
			{
				// シェイクの強度をフェードアウト
				double fade = m_shakeDuration / Max(0.001, m_shakeDuration + deltaTime);
				// より明確なシェイクパターン
				shakeOffset = Vec2(
					Math::Sin(Scene::Time() * 50.0) * m_shakeIntensity * fade,
					Math::Cos(Scene::Time() * 47.3) * m_shakeIntensity * fade
				);
				m_shakeDuration -= deltaTime;
			}

			// === ズーム補間 ===
			if (!m_isZoomLocked) // ズームがロックされていない場合のみ更新
			{
				double currentZoom = m_camera.getScale();
				if (m_zoomDuration > 0.0)
				{
					double t = 1.0 - (m_zoomDuration / m_totalZoomDuration);
					t = Clamp(t, 0.0, 1.0);
					double newZoom = Math::Lerp(currentZoom, m_targetZoom, t);
					m_camera.setScale(newZoom);
					m_zoomDuration -= deltaTime;
				}
				else
				{
					double newZoom = Math::Lerp(currentZoom, 1.0, 0.1);
					m_camera.setScale(newZoom);
				}
			}
			else // ズームがロックされている場合
			{
				double currentZoom = m_camera.getScale();
				if (m_zoomDuration > 0.0)
				{
					// 一時的なズーム変更を補間
					double t = 1.0 - (m_zoomDuration / m_totalZoomDuration);
					t = Clamp(t, 0.0, 1.0);
					double newZoom = Math::Lerp(currentZoom, m_targetZoom, t);
					m_camera.setScale(newZoom);
					m_zoomDuration -= deltaTime;
				}
				else
				{
					// ズーム変更が終わったら基準ズームに戻す
					double newZoom = Math::Lerp(currentZoom, m_lockedBaseZoom, 0.1);
					m_camera.setScale(newZoom);
				}
			}

			// === シェイクを適用した位置に移動（Y座標も制限） ===
			Vec2 finalPosition = actualTarget + shakeOffset;
			finalPosition.y = Clamp(finalPosition.y, m_minY, m_maxY);

			m_camera.jumpTo(finalPosition, m_camera.getScale());
			m_camera.update();
		}

		// スクリーン座標をワールド座標に変換
		[[nodiscard]] Vec2 screenToWorld(const Vec2& screenPos) const
		{
			// カメラの中心位置とスケールを取得
			const Vec2 center = m_camera.getCenter();
			const double scale = m_camera.getScale();
			// スクリーン中心からの相対座標を計算
			const Vec2 screenCenter = Scene::Center();
			const Vec2 offset = (screenPos - screenCenter) / scale;
			// ワールド座標を計算
			return center + offset;
		}

		// カーソルのワールド座標を取得
		[[nodiscard]] Vec2 getCursorWorldPos() const
		{
			return screenToWorld(Cursor::PosF());
		}

		[[nodiscard]] auto createTransformer() const
		{
			return m_camera.createTransformer();
		}

		[[nodiscard]] CameraMode getMode() const { return m_mode; }

		// カメラオフセットを取得（パララックス計算用）
		[[nodiscard]] Vec2 getCameraOffset() const
		{
			return m_camera.getCenter();
		}
	};
}
