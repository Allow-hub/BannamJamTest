#pragma once
#include <Siv3D.hpp>
#include "../../Stage/StageTypes.h"

namespace Jam::Domain::Editor
{
    // エディタ設定
    struct StageEditorConfig
    {
        int gridSize = 50;
        bool snapToGrid = true;
        bool showGrid = true;
        double cameraSpeed = 5.0;
    };

    // エディタ設定管理クラス
	    // Note: 本来はApplication層に置くべきだが、後方互換性のため一旦ここに残す
    class StageEditorSettings
    {
    private:
        StageEditorConfig m_config;
        
    public:
        StageEditorSettings() = default;
        
        int getGridSize() const { return m_config.gridSize; }
        void setGridSize(int size) { m_config.gridSize = size; }
        
        bool isSnapToGrid() const { return m_config.snapToGrid; }
        void toggleSnapToGrid() { m_config.snapToGrid = !m_config.snapToGrid; }
        
        bool isShowGrid() const { return m_config.showGrid; }
        void toggleShowGrid() { m_config.showGrid = !m_config.showGrid; }
        
        double getCameraSpeed() const { return m_config.cameraSpeed; }
        void setCameraSpeed(double speed) { m_config.cameraSpeed = Max(1.0, speed); }
        
        void reset() { m_config = StageEditorConfig{}; }
    };
}