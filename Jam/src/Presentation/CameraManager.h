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
		bool m_isZoomLocked = false;
		double m_lockedBaseZoom = 1.0;

		double m_minY = -std::numeric_limits<double>::infinity();
		double m_maxY = std::numeric_limits<double>::infinity();

	public:
		CameraManager(const Vec2& initPos = { 0, 0 });

		// === 設定 ===
		void setYLimits(double minY, double maxY);
		void setMinY(double minY);
		void setMaxY(double maxY);
		void resetYLimits();

		void setTarget(const Vec2& target);
		void setZoom(double zoom, double zoomDuration);
		void focusOn(const Vec2& point, double duration, double zoom = 1.0);
		void shake(double intensity, double duration);
		void resetToFollow();

		//特定の場所に固定
		void lockFocusOn(const Vec2& point, double zoom = 1.0);
		void updateFocusPoint(const Vec2& point);

		// === 更新・描画 ===
		void update(double deltaTime);

		// === 座標変換 ===
		[[nodiscard]] Vec2 screenToWorld(const Vec2& screenPos) const;
		[[nodiscard]] Vec2 getCursorWorldPos() const;

		[[nodiscard]] Transformer2D createTransformer() const;
		[[nodiscard]] CameraMode getMode() const;
		[[nodiscard]] Vec2 getCameraOffset() const;
	};
}
