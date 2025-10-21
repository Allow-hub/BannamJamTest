#pragma once
#include <Siv3D.hpp>
#include "../../Domain/Stage/BackgroundTypes.h"
#include "../../Infrastructure/TextureLoader.h"

/**
 * 背景描画クラス
 * パララックス効果付きの背景レンダリング
 */
namespace Jam::Presentation::Background {
    
    class BackgroundRenderer {
    private:
        Array<Jam::Domain::Background::BackgroundObject> m_backgroundObjects;
        bool m_isLoaded = false;
        
    public:
        BackgroundRenderer() = default;
        
        // 背景オブジェクトを設定
        void setBackgroundObjects(const Array<Jam::Domain::Background::BackgroundObject>& objects) {
            m_backgroundObjects = objects;
            m_isLoaded = true;
            Print << U"[BackgroundRenderer] ✅ Set " << objects.size() << U" background objects";
        }
        
        // 指定されたレイヤーの背景を描画
        void drawLayer(Jam::Domain::Background::ParallaxLayer layer, const Vec2& cameraOffset) const {
            if (!m_isLoaded) return;
            
            const double parallaxMultiplier = Jam::Domain::Background::getParallaxMultiplier(layer);
            const Vec2 parallaxOffset = calculateParallaxOffset(cameraOffset, parallaxMultiplier);
            
            for (const auto& bgObj : m_backgroundObjects) {
                if (bgObj.layer == layer) {
                    drawBackgroundObject(bgObj, parallaxOffset);
                }
            }
        }
        
        // すべての背景を描画順序通りに描画（Player描画位置を考慮）
        void drawAll(const Vec2& cameraOffset, std::function<void()> playerDrawCallback = nullptr) const {
            if (!m_isLoaded) return;
            
            // Back レイヤー描画
            drawLayer(Jam::Domain::Background::ParallaxLayer::Back, cameraOffset);
            
            // Middle レイヤー描画
            drawLayer(Jam::Domain::Background::ParallaxLayer::Middle, cameraOffset);
            
            // Player描画（コールバック）
            if (playerDrawCallback) {
                playerDrawCallback();
            }
            
            // Front レイヤー描画
            drawLayer(Jam::Domain::Background::ParallaxLayer::Front, cameraOffset);
        }
        
        // 読み込み状態の確認
        bool isLoaded() const { return m_isLoaded; }
        
        // 背景オブジェクト数の取得
        size_t getObjectCount() const { return m_backgroundObjects.size(); }
        
    private:
        // パララックスオフセットの計算
        Vec2 calculateParallaxOffset(const Vec2& cameraOffset, double multiplier) const {
            return Vec2(
                cameraOffset.x * multiplier,
                cameraOffset.y * multiplier
            );
        }
        
        // 単一の背景オブジェクトを描画
        void drawBackgroundObject(const Jam::Domain::Background::BackgroundObject& bgObj, const Vec2& offset) const {
            // テクスチャを取得
            const auto texture = Jam::Infrastructure::TextureLoader::getTexture(bgObj.textureName);
            if (!texture) {
                // テクスチャが見つからない場合はデバッグ用の矩形を描画
                const RectF drawRect = bgObj.rect.movedBy(-offset);
                drawRect.drawFrame(2, Palette::Magenta);
                return;
            }
            
            // 描画位置の計算
            const RectF drawRect = bgObj.rect.movedBy(-offset);
            
            // 透明度を適用して描画
            if (bgObj.opacity < 1.0) {
                texture->resized(drawRect.size).drawAt(drawRect.center(), ColorF(1.0, bgObj.opacity));
            } else {
                texture->resized(drawRect.size).drawAt(drawRect.center());
            }
            
            // リピート描画の場合の処理（将来的な拡張用）
            if (bgObj.isRepeating) {
                // TODO: タイル状の繰り返し描画を実装
            }
        }
    };
}
