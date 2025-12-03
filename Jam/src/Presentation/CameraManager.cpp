#include "Presentation/CameraManager.h"

namespace Jam::Presentation
{
	using namespace s3d;

	CameraManager::CameraManager(const Vec2& initPos)
	{
		m_camera = Camera2D(initPos, 1.0, CameraControl::None_);
		m_target = initPos;
	}

	void CameraManager::setYLimits(double minY, double maxY)
	{
		m_minY = minY;
		m_maxY = maxY;
	}

	void CameraManager::setMinY(double minY) { m_minY = minY; }
	void CameraManager::setMaxY(double maxY) { m_maxY = maxY; }

	void CameraManager::resetYLimits()
	{
		m_minY = -std::numeric_limits<double>::infinity();
		m_maxY = std::numeric_limits<double>::infinity();
	}

	void CameraManager::setTarget(const Vec2& target) { m_target = target; }

	void CameraManager::setZoom(double zoom, double zoomDuration)
	{
		if (m_isZoomLocked)
			m_targetZoom = Clamp(zoom * m_lockedBaseZoom, 0.01, 2.0);
		else
			m_targetZoom = Clamp(zoom, 0.01, 2.0);

		m_zoomDuration = zoomDuration;
		m_totalZoomDuration = zoomDuration;
	}

	void CameraManager::focusOn(const Vec2& point, double duration, double zoom)
	{
		m_mode = CameraMode::FocusPoint;
		m_focusPoint = point;
		m_modeDuration = duration;

		if (m_isZoomLocked)
			m_targetZoom = zoom * m_lockedBaseZoom;
		else
			m_targetZoom = zoom;
	}

	void CameraManager::shake(double intensity, double duration)
	{
		m_shakeIntensity = intensity;
		m_shakeDuration = duration;
	}

	void CameraManager::resetToFollow()
	{
		m_mode = CameraMode::FollowPlayer;
		m_modeDuration = 0.0;
		m_isZoomLocked = false;
	}

	void CameraManager::lockFocusOn(const Vec2& point, double zoom)
	{
		m_mode = CameraMode::FocusPoint;
		m_focusPoint = point;
		m_modeDuration = std::numeric_limits<double>::infinity();
		m_targetZoom = Clamp(zoom, 0.01, 2.0);
		m_lockedBaseZoom = m_targetZoom;
		m_isZoomLocked = true;

		m_camera.setScale(m_targetZoom);
		m_zoomDuration = 0.0;
	}

	void CameraManager::updateFocusPoint(const Vec2& point)
	{
		if (m_mode == CameraMode::FocusPoint)
			m_focusPoint = point;
	}

	void CameraManager::update(double deltaTime)
	{
		if (m_modeDuration > 0.0 && m_modeDuration != std::numeric_limits<double>::infinity())
		{
			m_modeDuration -= deltaTime;
			if (m_modeDuration <= 0.0) resetToFollow();
		}

		Vec2 actualTarget = (m_mode == CameraMode::FocusPoint) ? m_focusPoint : m_target;
		actualTarget.y = Clamp(actualTarget.y, m_minY, m_maxY);

		Vec2 shakeOffset = Vec2::Zero();
		if (m_shakeDuration > 0.0)
		{
			double fade = m_shakeDuration / Max(0.001, m_shakeDuration + deltaTime);
			shakeOffset = {
				Math::Sin(Scene::Time() * 50.0) * m_shakeIntensity * fade,
				Math::Cos(Scene::Time() * 47.3) * m_shakeIntensity * fade
			};
			m_shakeDuration -= deltaTime;
		}

		if (!m_isZoomLocked)
		{
			double currentZoom = m_camera.getScale();
			if (m_zoomDuration > 0.0)
			{
				double t = 1.0 - (m_zoomDuration / m_totalZoomDuration);
				double newZoom = Math::Lerp(currentZoom, m_targetZoom, Clamp(t, 0.0, 1.0));
				m_camera.setScale(newZoom);
				m_zoomDuration -= deltaTime;
			}
			else
			{
				if (m_mode == CameraMode::FocusPoint)
				{
					m_camera.setScale(Math::Lerp(currentZoom, m_targetZoom, 0.2));
				}
				else
				{
					m_camera.setScale(Math::Lerp(currentZoom, 1.0, 0.1));
				}
			}
		}
		else
		{
			double currentZoom = m_camera.getScale();
			if (m_zoomDuration > 0.0)
			{
				double t = 1.0 - (m_zoomDuration / m_totalZoomDuration);
				double newZoom = Math::Lerp(currentZoom, m_targetZoom, Clamp(t, 0.0, 1.0));
				m_camera.setScale(newZoom);
				m_zoomDuration -= deltaTime;
			}
			else
			{
				m_camera.setScale(Math::Lerp(currentZoom, m_lockedBaseZoom, 0.1));
			}
		}

		Vec2 finalPosition = actualTarget + shakeOffset;
		finalPosition.y = Clamp(finalPosition.y, m_minY, m_maxY);
		m_camera.jumpTo(finalPosition, m_camera.getScale());
		m_camera.update();
	}

	Vec2 CameraManager::screenToWorld(const Vec2& screenPos) const
	{
		const Vec2 center = m_camera.getCenter();
		const double scale = m_camera.getScale();
		const Vec2 screenCenter = Scene::Center();
		const Vec2 offset = (screenPos - screenCenter) / scale;
		return center + offset;
	}

	Vec2 CameraManager::getCursorWorldPos() const
	{
		return screenToWorld(Cursor::PosF());
	}

	Transformer2D CameraManager::createTransformer() const
	{
		return m_camera.createTransformer();
	}

	CameraMode CameraManager::getMode() const { return m_mode; }

	Vec2 CameraManager::getCameraOffset() const { return m_camera.getCenter(); }
}
