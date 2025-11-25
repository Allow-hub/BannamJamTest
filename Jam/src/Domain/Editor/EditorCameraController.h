#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Editor
{
    // エディタカメラコントローラ
    class EditorCameraController
    {
    private:
        Camera2D m_camera{Vec2{0, 0}, 1.0};
        double m_speed = 5.0;
        
        static constexpr double MIN_SCALE = 0.1;
        static constexpr double MAX_SCALE = 4.0;
        static constexpr double ZOOM_FACTOR = 1.1;
        
    public:
        void update();
        
        Vec2 screenToWorld(const Vec2& screenPos) const;
        
        auto createTransformer() const { return m_camera.createTransformer(); }
        const Camera2D& getCamera() const { return m_camera; }
        
        void setSpeed(double speed) { m_speed = Max(1.0, speed); }
        double getSpeed() const { return m_speed; }
        
    private:
        void updateMovement();
        void updateZoom();
    };
}
