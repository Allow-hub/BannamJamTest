#pragma once
#include <Siv3D.hpp>

namespace Jam::Presentation::Editor
{
    template<typename TService>
    class EditorRendererBase
    {
    protected:
        TService* m_service = nullptr;
        Font m_font{20};
        Font m_smallFont{16};
        
        mutable bool m_isPanelCollapsed = false;
        mutable double m_scrollY = 0.0;
        
        // GUIレイアウト定数
        static constexpr int PANEL_WIDTH = 300;
        static constexpr int COLLAPSED_PANEL_WIDTH = 40;
        static constexpr int GUI_PADDING = 10;
        static constexpr int GUI_PADDING_15 = 15;
        static constexpr int GUI_SPACING_20 = 20;
        static constexpr int GUI_SPACING_25 = 25;
        static constexpr int GUI_SPACING_30 = 30;
        static constexpr int GUI_SPACING_35 = 35;
        static constexpr int GUI_SPACING_40 = 40;
        static constexpr int GUI_SPACING_45 = 45;
        static constexpr int GUI_BUTTON_WIDTH = 270;
        static constexpr int GUI_BUTTON_WIDTH_WIDE = 280;
        static constexpr int GUI_DROPDOWN_BUTTON_WIDTH = 260;
        static constexpr int GUI_SMALL_BUTTON_WIDTH = 130;
        static constexpr int GUI_BUTTON_HEIGHT = 30;
        static constexpr int TITLE_Y = 10;
        static constexpr int TITLE_OFFSET_X = 45;
        static constexpr int CONTENT_START_Y = 55;
        static constexpr int SCROLL_SPEED = 20;
        
        int getPanelX() const
        {
            const int actualWidth = m_isPanelCollapsed ? COLLAPSED_PANEL_WIDTH : PANEL_WIDTH;
            return Scene::Width() - actualWidth;
        }
        
        void handlePanelScroll() const
        {
            if (isMouseOverPanel())
                m_scrollY = Max(0.0, m_scrollY + Mouse::Wheel() * SCROLL_SPEED);
        }
        
        void drawTitleAndToggle(const String& title) const
        {
            if (drawCollapseButton(getPanelX()))
                m_isPanelCollapsed = !m_isPanelCollapsed;
            m_font(title).draw(getPanelX() + TITLE_OFFSET_X, TITLE_Y, Palette::White);
        }
        
        int getContentStartY() const
        {
            return CONTENT_START_Y; // タイトル領域の高さ
        }
        
        int drawCameraSettings(int y) const
        {
            static constexpr double MIN_CAMERA_SPEED = 1.0;
            static constexpr double MAX_CAMERA_SPEED = 20.0;
            static constexpr int SLIDER_LABEL_WIDTH = 80;
            static constexpr int SLIDER_WIDTH = 180;
            
            SimpleGUI::Headline(U"カメラ操作", Vec2{getPanelX() + GUI_PADDING, y});
            y += GUI_SPACING_35;
            
            double cameraSpeed = m_service->getCameraSpeed();
            if (SimpleGUI::Slider(U"速度: {:.1f}"_fmt(cameraSpeed), cameraSpeed, MIN_CAMERA_SPEED, MAX_CAMERA_SPEED, Vec2{getPanelX() + GUI_PADDING, y}, SLIDER_LABEL_WIDTH, SLIDER_WIDTH))
                m_service->setCameraSpeed(cameraSpeed);
            y += GUI_SPACING_40;
            
            m_smallFont(U"WASD: 移動").draw(getPanelX() + GUI_PADDING_15, y, Palette::White);
            y += GUI_SPACING_25;
            m_smallFont(U"ホイール: ズーム").draw(getPanelX() + GUI_PADDING_15, y, Palette::White);
            y += GUI_SPACING_40;
            return y;
        }
        
        int drawModeDisplay(int y) const
        {
            SimpleGUI::Headline(U"モード切り替え", Vec2{getPanelX() + GUI_PADDING, y});
            y += GUI_SPACING_35;
            m_smallFont(U"1: 選択").draw(getPanelX() + GUI_PADDING_15, y, Palette::White);
            y += GUI_SPACING_25;
            m_smallFont(U"2: 配置").draw(getPanelX() + GUI_PADDING_15, y, Palette::White);
            y += GUI_SPACING_25;
            m_smallFont(U"3: 削除").draw(getPanelX() + GUI_PADDING_15, y, Palette::White);
            y += GUI_SPACING_40;
            return y;
        }
        
        void drawBasicPanel() const
        {
            drawPanelBackground(PANEL_WIDTH);
        }
        
    public:
        virtual ~EditorRendererBase() = default;
        
        void init(TService* service)
        {
            m_service = service;
        }
        
        // パネルが折りたたまれているかを取得
        bool isPanelCollapsed() const { return m_isPanelCollapsed; }
        
        // マウスがパネル上にあるかを取得（折りたたみ状態を考慮）
        bool isMouseOverPanel() const
        {
            return Cursor::Pos().x >= getPanelX();
        }
        
        // ===== 派生クラスで実装 =====
        virtual void drawView() const = 0;
        virtual void drawGUIPanel() const = 0;
        
    protected:
        // 共通のGUI要素
        void drawPanelBackground(int panelWidth) const
        {
            const int actualWidth = m_isPanelCollapsed ? COLLAPSED_PANEL_WIDTH : panelWidth;
            const int panelX = Scene::Width() - actualWidth;
            RectF{panelX, 0, actualWidth, Scene::Height()}.draw(ColorF{0.15, 0.15, 0.15, 0.95});
        }
        
        auto createPanelTransformer() const
        {
            const int panelX = Scene::Width() - PANEL_WIDTH;
            return Transformer2D{Mat3x2::Translate(0, -m_scrollY), TransformCursor::Yes, Transformer2D::Target::PushLocal};
        }
        
        bool drawCollapseButton(int panelX) const
        {
            return SimpleGUI::Button(
                m_isPanelCollapsed ? U"◀" : U"▶", 
                Vec2{panelX + GUI_PADDING, TITLE_Y}, 
                GUI_BUTTON_HEIGHT
            );
        }
        
        void drawModeIndicator(int x, int y, const String& modeName) const
        {
            m_smallFont(U"モード: " + modeName).draw(x, y, Palette::Yellow);
        }
        
        // 見出しを描画（ユーザーが触れない部分）
        int drawSectionHeader(const String& text, int y) const
        {
            static constexpr int HEADER_HEIGHT = 30;
            static constexpr int HEADER_MARGIN = 10;
            
            const int panelX = getPanelX();
            const int panelWidth = PANEL_WIDTH;
            
            // 背景を描画
            RectF{panelX + GUI_PADDING, y, panelWidth - HEADER_MARGIN, HEADER_HEIGHT}.draw(ColorF{0.25, 0.35, 0.45, 0.8});
            
            // テキストを中央揃えで描画
            m_font(text).drawAt(panelX + panelWidth / 2, y + GUI_PADDING_15, ColorF{0.9, 0.95, 1.0});
            
            return y + GUI_SPACING_40;  // 見出しの高さ + 下間隔
        }
        
        // サブ見出しを描画（少し小さめ）
        int drawSubHeader(const String& text, int y) const
        {
            const int panelX = getPanelX();
            m_smallFont(text).draw(panelX + GUI_PADDING, y, ColorF{0.7, 0.8, 0.9});
            return y + GUI_SPACING_25;
        }
        
        // 項目間の間隔を取得（画面サイズに応じて調整）
        int getLargeSpacing() const
        {
            return Max(GUI_SPACING_30, Scene::Height() / 30);  // 最低30px、画面高さに応じて増加
        }
        
        int getSmallSpacing() const
        {
            return Max(GUI_SPACING_20, Scene::Height() / 40);
        }
    };
}