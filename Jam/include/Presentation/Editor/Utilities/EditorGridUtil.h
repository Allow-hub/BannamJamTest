#pragma once
#include <Siv3D.hpp>

namespace Jam::Presentation::Editor
{
    // エディタグリッド描画ユーティリティクラス
    class EditorGridUtil
    {
    public:
        // グリッドを描画
        static void drawGrid(const Camera2D& camera, int gridSize);
        
        // グリッドと軸を描画
        static void drawGridWithAxes(const Camera2D& camera, int gridSize);
        
    private:
        // 表示範囲を計算
        struct ViewBounds
        {
            int startX;
            int endX;
            int startY;
            int endY;
            double worldStartY;
            double worldEndY;
        };
        
        static ViewBounds calculateViewBounds(const Camera2D& camera, int gridSize);
        
        // グリッド線を描画
        static void drawGridLines(const ViewBounds& bounds, int gridSize, const ColorF& gridColor);
        
        // 原点軸を描画
        static void drawOriginAxes(const ViewBounds& bounds, int gridSize);
        
        // デフォルトのグリッド色
        static constexpr ColorF DEFAULT_GRID_COLOR = ColorF{0.3, 0.3, 0.3, 0.5};
        static constexpr ColorF AXIS_X_COLOR = ColorF{1.0, 0.0, 0.0}; // 赤: X軸
        static constexpr ColorF AXIS_Y_COLOR = ColorF{0.0, 1.0, 0.0}; // 緑: Y軸
    };
}