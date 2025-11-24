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
        
        static constexpr int PANEL_WIDTH = 300;
        
        int getPanelX() const
        {
            const int actualWidth = m_isPanelCollapsed ? 40 : PANEL_WIDTH;
            return Scene::Width() - actualWidth;
        }
        
        void handlePanelScroll() const
        {
            if (isMouseOverPanel())
            {
                m_scrollY = Max(0.0, m_scrollY + Mouse::Wheel() * 20.0);
            }
        }
        
        void drawTitleAndToggle(const String& title) const
        {
            int y = 10;
            if (drawCollapseButton(getPanelX()))
            {
                m_isPanelCollapsed = !m_isPanelCollapsed;
            }
            m_font(title).draw(getPanelX() + 45, y, Palette::White);
        }
        
        int getContentStartY() const
        {
            return 55; // タイトル領域の高さ
        }
        
        int drawCameraSettings(int y) const
        {
            SimpleGUI::Headline(U"カメラ操作", Vec2{getPanelX() + 10, y});
            y += 35;
            
            double cameraSpeed = m_service->getCameraSpeed();
            if (SimpleGUI::Slider(U"速度: {:.1f}"_fmt(cameraSpeed), cameraSpeed, 1.0, 20.0, Vec2{getPanelX() + 10, y}, 80, 180))
            {
                m_service->setCameraSpeed(cameraSpeed);
            }
            y += 40;
            
            m_smallFont(U"WASD: 移動").draw(getPanelX() + 15, y, Palette::White);
            y += 25;
            m_smallFont(U"ホイール: ズーム").draw(getPanelX() + 15, y, Palette::White);
            y += 40;
            return y;
        }
        
        int drawModeDisplay(int y) const
        {
            SimpleGUI::Headline(U"モード切り替え", Vec2{getPanelX() + 10, y});
            y += 35;
            m_smallFont(U"1: 選択").draw(getPanelX() + 15, y, Palette::White);
            y += 25;
            m_smallFont(U"2: 配置").draw(getPanelX() + 15, y, Palette::White);
            y += 25;
            m_smallFont(U"3: 削除").draw(getPanelX() + 15, y, Palette::White);
            y += 40;
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
            const int actualWidth = m_isPanelCollapsed ? 40 : panelWidth;
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
                Vec2{panelX + 5, 10}, 
                30
            );
        }
        
        void drawModeIndicator(int x, int y, const String& modeName) const
        {
            m_smallFont(U"モード: " + modeName).draw(x, y, Palette::Yellow);
        }
        
        // 見出しを描画（ユーザーが触れない部分）
        int drawSectionHeader(const String& text, int y) const
        {
            const int panelX = getPanelX();
            const int panelWidth = PANEL_WIDTH;
            
            // 背景を描画
            RectF{panelX + 5, y, panelWidth - 10, 30}.draw(ColorF{0.25, 0.35, 0.45, 0.8});
            
            // テキストを中央揃えで描画
            m_font(text).drawAt(panelX + panelWidth / 2, y + 15, ColorF{0.9, 0.95, 1.0});
            
            return y + 40;  // 見出しの高さ + 下間隔
        }
        
        // サブ見出しを描画（少し小さめ）
        int drawSubHeader(const String& text, int y) const
        {
            const int panelX = getPanelX();
            m_smallFont(text).draw(panelX + 10, y, ColorF{0.7, 0.8, 0.9});
            return y + 25;
        }
        
        // 項目間の間隔を取得（画面サイズに応じて調整）
        int getItemSpacing() const
        {
            return Max(30, Scene::Height() / 30);  // 最低30px、画面高さに応じて増加
        }
        
        int getSmallSpacing() const
        {
            return Max(20, Scene::Height() / 40);
        }
    };
}