#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Editor
{
	struct EditorConfig
	{
		static constexpr int DEFAULT_GRID_SIZE = 50;
		static constexpr double DEFAULT_CAMERA_SPEED = 5.0;
		static constexpr double MIN_CAMERA_SPEED = 1.0;

		int gridSize = DEFAULT_GRID_SIZE;
		bool snapToGrid = true;
		bool showGrid = true;
		double cameraSpeed = DEFAULT_CAMERA_SPEED;
	};

	// StageEditorとEnemyEditorで共有される設定
	class EditorSettings
	{
	private:
		EditorConfig m_config;

	public:
		EditorSettings() = default;

		// ===== グリッド設定 =====
		int getGridSize() const { return m_config.gridSize; }
		void setGridSize(int size) { m_config.gridSize = size; }

		bool isSnapToGrid() const { return m_config.snapToGrid; }
		void toggleSnapToGrid() { m_config.snapToGrid = !m_config.snapToGrid; }

		bool isShowGrid() const { return m_config.showGrid; }
		void toggleShowGrid() { m_config.showGrid = !m_config.showGrid; }

		// ===== カメラ設定 =====
		double getCameraSpeed() const { return m_config.cameraSpeed; }
		void setCameraSpeed(double speed) { m_config.cameraSpeed = Max(m_config.MIN_CAMERA_SPEED, speed); }

		// ===== リセット =====
		void reset() { m_config = EditorConfig{}; }
	};
}
