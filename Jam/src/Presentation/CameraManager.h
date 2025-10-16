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
	public:
		CameraManager(const Vec2& initPos = { 0, 0 })
		{
			m_camera = Camera2D(initPos, 1.0, CameraControl::None_);
			m_target = initPos;
		}
		void setTarget(const Vec2& target)
		{
			m_target = target;
		}
		void setZoom(double zoom, double zoomDuration)
		{
			m_targetZoom = Clamp(zoom, 0.5, 2.0);
			m_zoomDuration = zoomDuration;
			m_totalZoomDuration = zoomDuration;
		}
		void focusOn(const Vec2& point, double duration, double zoom = 1.0)
		{
			m_mode = CameraMode::FocusPoint;
			m_focusPoint = point;
			m_modeDuration = duration;
			m_targetZoom = zoom;
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
		}

		void update(double deltaTime)
		{
			// === モード更新 ===
			if (m_modeDuration > 0.0)
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

			// === シェイクを適用した位置に移動 ===
			m_camera.jumpTo(actualTarget + shakeOffset, m_camera.getScale());
			m_camera.update();
		}

		[[nodiscard]] auto createTransformer() const
		{
			return m_camera.createTransformer();
		}
		[[nodiscard]] CameraMode getMode() const { return m_mode; }
	};
}
