#include "EditorCameraController.h"

namespace Jam::Domain::Editor
{
    void EditorCameraController::update()
    {
        if (KeyControl.pressed() || KeyAlt.pressed()) {
            return;
        }
        
        updateMovement();
        updateZoom();
    }
    
    Vec2 EditorCameraController::screenToWorld(const Vec2& screenPos) const
    {
        static constexpr double HALF = 0.5;
        const Vec2 screenCenter(Scene::Width() * HALF, Scene::Height() * HALF);
        return m_camera.getCenter() + (screenPos - screenCenter) / m_camera.getScale();
    }
    
    void EditorCameraController::updateMovement()
    {
        // テキスト入力中はカメラ移動しない
        if (TextInput::GetEditingText()) {
            return;
        }
        
        Vec2 movement{0, 0};
        if (KeyW.pressed() || KeyUp.pressed()) movement.y -= m_speed;
        if (KeyS.pressed() || KeyDown.pressed()) movement.y += m_speed;
        if (KeyA.pressed() || KeyLeft.pressed()) movement.x -= m_speed;
        if (KeyD.pressed() || KeyRight.pressed()) movement.x += m_speed;
        
        if (movement != Vec2{0, 0}) {
            m_camera.setCenter(m_camera.getCenter() + movement);
        }
    }
    
    void EditorCameraController::updateZoom()
    {
        // GUIパネル上ではズームしない
        static constexpr int PANEL_WIDTH = 300;
        const int panelX = Scene::Width() - PANEL_WIDTH;
        if (Cursor::Pos().x >= panelX) {
            return;
        }
        
        const int wheel = Mouse::Wheel();
        if (wheel > 0) {
            m_camera.setScale(Max(MIN_SCALE, m_camera.getScale() / ZOOM_FACTOR));
        } else if (wheel < 0) {
            m_camera.setScale(Min(MAX_SCALE, m_camera.getScale() * ZOOM_FACTOR));
        }
    }
}
