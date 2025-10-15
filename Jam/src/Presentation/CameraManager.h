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

		void setZoom(double zoom,double zoomDuration)
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
			if (m_modeDuration > 0.0)
			{
				m_modeDuration -= deltaTime;
				if (m_modeDuration <= 0.0)
				{
					resetToFollow();
				}
			}

			Vec2 actualTarget = m_target;
			double lerpSpeed = 0.15;

			switch (m_mode)
			{
			case CameraMode::FollowPlayer:
				actualTarget = m_target;
				break;

			case CameraMode::FocusPoint:
				actualTarget = m_focusPoint;
				lerpSpeed = 0.08;
				break;

			case CameraMode::Cinematic:
				break;
			}

			if (m_shakeDuration > 0.0)
			{
				m_shakeDuration -= deltaTime;
				Vec2 shakeOffset = {
					Random(-m_shakeIntensity, m_shakeIntensity),
					Random(-m_shakeIntensity, m_shakeIntensity)
				};
				actualTarget += shakeOffset;
			}

			Vec2 current = m_camera.getCenter();
			Vec2 newPos = Math::Lerp(current, actualTarget, lerpSpeed);
			m_camera.setCenter(newPos);
			// ===== ズーム補間（時間ベース） =====
			if (m_zoomDuration > 0.0)
			{
				m_zoomDuration -= deltaTime;

				double t = 1.0 - (m_zoomDuration / m_totalZoomDuration);
				t = Clamp(t, 0.0, 1.0);

				double currentZoom = m_camera.getScale();
				double newZoom = Math::Lerp(currentZoom, m_targetZoom, t);
				m_camera.setScale(newZoom);
			}
			else
			{
				m_targetZoom = 1.0;

				// 通常の安定ズーム補間
				double currentZoom = m_camera.getScale();
				double newZoom = Math::Lerp(currentZoom, m_targetZoom, 0.1);
				m_camera.setScale(newZoom);
			}

			m_camera.update();
		}

		[[nodiscard]] auto createTransformer() const
		{
			return m_camera.createTransformer();
		}

		[[nodiscard]] CameraMode getMode() const { return m_mode; }
	};
}
