#include "Presentation/Editor/Utilities/EditorGridUtil.h"

namespace Jam::Presentation::Editor
{
    void EditorGridUtil::drawGrid(const Camera2D& camera, int gridSize)
    {
        const ViewBounds bounds = calculateViewBounds(camera, gridSize);
        drawGridLines(bounds, gridSize, DEFAULT_GRID_COLOR);
    }
    
    void EditorGridUtil::drawGridWithAxes(const Camera2D& camera, int gridSize)
    {
        const ViewBounds bounds = calculateViewBounds(camera, gridSize);
        drawGridLines(bounds, gridSize, DEFAULT_GRID_COLOR);
        drawOriginAxes(bounds, gridSize);
    }
    
    EditorGridUtil::ViewBounds EditorGridUtil::calculateViewBounds(const Camera2D& camera, int gridSize)
    {
        const Vec2 center = camera.getCenter();
        const double scale = camera.getScale();
        const double viewWidth = Scene::Width() / scale;
        const double viewHeight = Scene::Height() / scale;
        
        ViewBounds bounds;
        bounds.startX = static_cast<int>((center.x - viewWidth / 2) / gridSize) - 1;
        bounds.endX = static_cast<int>((center.x + viewWidth / 2) / gridSize) + 1;
        bounds.startY = static_cast<int>((center.y - viewHeight / 2) / gridSize) - 1;
        bounds.endY = static_cast<int>((center.y + viewHeight / 2) / gridSize) + 1;
        bounds.worldStartY = bounds.startY * gridSize;
        bounds.worldEndY = bounds.endY * gridSize;
        
        return bounds;
    }
    
    void EditorGridUtil::drawGridLines(const ViewBounds& bounds, int gridSize, const ColorF& gridColor)
    {
        // 縦線を描画
        for (int x = bounds.startX; x <= bounds.endX; ++x)
        {
            double xPos = x * gridSize;
            Line{xPos, bounds.worldStartY, xPos, bounds.worldEndY}.draw(0.5, gridColor);
        }
        
        // 横線を描画
        for (int y = bounds.startY; y <= bounds.endY; ++y)
        {
            double yPos = y * gridSize;
            Line{bounds.startX * gridSize, yPos, bounds.endX * gridSize, yPos}.draw(0.5, gridColor);
        }
    }
    
    void EditorGridUtil::drawOriginAxes(const ViewBounds& bounds, int gridSize)
    {
        // Y軸（縦の赤線）
        Line{0, bounds.worldStartY, 0, bounds.worldEndY}.draw(2.0, AXIS_X_COLOR);
        
        // X軸（横の緑線）
        Line{bounds.startX * gridSize, 0, bounds.endX * gridSize, 0}.draw(2.0, AXIS_Y_COLOR);
    }
}