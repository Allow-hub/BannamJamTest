#pragma once
#include <Siv3D.hpp>
#include "StageTypes.h"

namespace Jam::Domain::Stage {
	
	/**
	 * ステージデバッグ表示機能
	 * 当たり判定の可視化とデバッグ情報表示
	 */
    namespace DebugConfig {
        // デバッグビルド時のみ有効（リリースビルドでは完全に無効化）
#ifdef _DEBUG
        // 当たり判定ボックスの表示制御（true: 表示, false: 非表示）
        constexpr bool SHOW_COLLISION_BOXES = false;
        // 当たり判定タイプラベルの表示制御（true: 表示, false: 非表示）
        constexpr bool SHOW_COLLISION_LABELS = false;
#else
        // リリースビルドでは強制的に無効
        constexpr bool SHOW_COLLISION_BOXES = false;
        constexpr bool SHOW_COLLISION_LABELS = false;
#endif
        
        // デバッグ表示色設定
        namespace Colors {
            constexpr Color SOLID_OUTLINE = Palette::Red;
            constexpr Color PLATFORM_OUTLINE = Palette::Green;
            constexpr Color HAZARD_OUTLINE = Palette::Orange;
            constexpr Color TRIGGER_OUTLINE = Palette::Blue;
            constexpr Color BREAKABLE_OUTLINE = Palette::Purple;
            constexpr Color DEFAULT_OUTLINE = Palette::White;
        }
        
        // 当たり判定タイプ別の表示色取得
        inline Color getCollisionOutlineColor(CollisionType type) {
            switch (type) {
            case CollisionType::Solid: return Colors::SOLID_OUTLINE;
            case CollisionType::Platform: return Colors::PLATFORM_OUTLINE;
            case CollisionType::Hazard: return Colors::HAZARD_OUTLINE;
            case CollisionType::Trigger: return Colors::TRIGGER_OUTLINE;
            case CollisionType::Breakable: return Colors::BREAKABLE_OUTLINE;
            default: return Colors::DEFAULT_OUTLINE;
            }
        }
    }
    
    // デバッグ描画ユーティリティ
    namespace DebugRenderer {
        // 単一オブジェクトのデバッグ描画
        inline void drawObjectDebug(const StageObject& obj) {
            if constexpr (!DebugConfig::SHOW_COLLISION_BOXES) return;
            
            // 当たり判定枠の描画
            const Color outlineColor = DebugConfig::getCollisionOutlineColor(obj.type);
            obj.rect.drawFrame(2, outlineColor);
            
            // タイプラベル表示
            if constexpr (DebugConfig::SHOW_COLLISION_LABELS) {
                const String label = collisionTypeToString(obj.type);
                const Vec2 labelPos = obj.rect.pos.movedBy(2, -18);
                SimpleGUI::GetFont()(label).draw(labelPos, outlineColor);
            }
        }
        
        // オブジェクト配列のデバッグ描画
        inline void drawCollectionDebug(const Array<StageObject>& objects) {
            if constexpr (!DebugConfig::SHOW_COLLISION_BOXES) return;
            
            for (const auto& obj : objects) {
                drawObjectDebug(obj);
            }
        }
    }
}