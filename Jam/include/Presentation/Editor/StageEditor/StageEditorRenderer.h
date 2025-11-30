#pragma once
#include "Presentation/Editor/Base/EditorRendererBase.h"
#include "UseCase/Editor/StageEditor/StageEditorService.h"
#include "Presentation/Editor/Utilities/EditorGridUtil.h"

namespace Jam::Presentation::Editor
{
    // ステージエディタを描画するクラス
    class StageEditorRenderer : public EditorRendererBase<UseCase::Editor::StageEditorService>
    {
    private:
        static constexpr double FRAME_THICKNESS = 2.0;
        static constexpr double PLAYER_SPAWN_RADIUS = 20.0;
        
        mutable bool m_isStageTypeDropdownOpen = false;
        mutable bool m_isMovementTypeDropdownOpen = false;
        mutable bool m_isTextureDropdownOpen = false;
        mutable bool m_switchToEnemyEditor = false;
        
        mutable TextEditState m_metadataTextEdit;
        mutable TextEditState m_distanceTextEdit;
        mutable TextEditState m_speedTextEdit;
        mutable TextEditState m_damageTextEdit;
        
    public:
        void drawView() const override
        {
            const auto& camera = this->m_service->getCamera();
            auto transformer = camera.createTransformer();
            
            // グリッドは常に表示
            EditorGridUtil::drawGridWithAxes(camera, this->m_service->getSettings().getGridSize());
            
            drawObjects(camera, this->m_service->getManager().getAllObjects());
            
            // 配置モードのドラッグ矩形
            if (auto dragRect = this->m_service->getDragRect())
            {
                dragRect->draw(Palette::Lime.withAlpha(0.3));
                dragRect->drawFrame(FRAME_THICKNESS, Palette::Lime.withAlpha(0.8));
            }
            
            // 選択モードのドラッグ矩形
            if (auto selectionRect = this->m_service->getSelectionDragRect())
            {
                selectionRect->draw(Palette::Deepskyblue.withAlpha(0.2));
                selectionRect->drawFrame(FRAME_THICKNESS, Palette::Deepskyblue.withAlpha(0.8));
            }
        }
        
        void drawGUIPanel() const override
        {
            this->handlePanelScroll();
            this->drawBasicPanel();
            
            // 折りたたみボタンとタイトル（スクロール対象外）
            this->drawTitleAndToggle(U"ステージエディタ");
            
            if (this->m_isPanelCollapsed) return;
            
            {
                auto transformer = this->createPanelTransformer();
                
                int y = this->getContentStartY();
                
                if (SimpleGUI::Button(U"敵エディタに切り替え", Vec2{this->getPanelX() + 10, y}, 280))
                    m_switchToEnemyEditor = true;
                y += 45;
                
                y = this->drawCameraSettings(y);
                y = drawCurrentMode(y);
                y = drawOtherObjectsSelector(y);
                y = drawStageTypeSelector(y);
                y = drawTextureSelector(y);
                y = drawMovementSettings(y);
                y = drawDamageSettings(y);
                y = drawMetadataEdit(y);
                y = drawSelectionInfo(y);
            }
        }
        
        bool isEditorSwitchRequested() const { return m_switchToEnemyEditor; }
        void resetEditorSwitchRequest() const { m_switchToEnemyEditor = false; }
        
        // テキスト入力中かどうかを判定
        bool isTextInputActive() const
        {
            return m_metadataTextEdit.active || m_distanceTextEdit.active || 
                   m_speedTextEdit.active || m_damageTextEdit.active;
        }
        
        // テキスト入力を全て無効化
        void clearTextInputFocus() const
        {
            m_metadataTextEdit.active = false;
            m_distanceTextEdit.active = false;
            m_speedTextEdit.active = false;
            m_damageTextEdit.active = false;
        }
        
        // StageEditorSceneから呼び出すためpublicに
        void drawObject(const Domain::Editor::StageEditorObject& obj, bool isSelected) const;
        
    private:
        int drawCurrentMode(int y) const;
        int drawStageTypeSelector(int y) const;
        int drawTextureSelector(int y) const;
        int drawOtherObjectsSelector(int y) const;
        int drawMovementSettings(int y) const;
        int drawDamageSettings(int y) const;
        int drawMetadataEdit(int y) const;
        int drawSelectionInfo(int y) const;
        
        void drawObjects(const Camera2D& camera, const Array<Domain::Editor::StageEditorObject>& objects) const;
        void drawMovementGuide(const Domain::Stage::StageObject& obj) const;
        
        ColorF getGroundSideColor(Domain::Stage::GroundSide side) const;
        String getStageTypeName(Domain::Stage::StageType type) const;
    };
}