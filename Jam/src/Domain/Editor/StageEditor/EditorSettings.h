#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Editor
{
	struct EditorConfig
	{
		int gridSize = 50;
		bool snapToGrid = true;
		bool showGrid = true;
		double cameraSpeed = 5.0;
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
		void setCameraSpeed(double speed) { m_config.cameraSpeed = Max(1.0, speed); }

		// ===== リセット =====
		void reset() { m_config = EditorConfig{}; }
	};
}
