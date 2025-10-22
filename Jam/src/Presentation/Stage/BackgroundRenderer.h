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



		// 特定のテクスチャを使用してBackレイヤーとして描画（既存のJSONなしの場合用）
		void drawBackgroundTexture(const String& textureName, const Vec2& cameraOffset,
								 const Vec2& position = Vec2(-500, -800),
								 const Size& size = Size(4000, 2000)) const {
			// BG.pngテクスチャを取得
			const auto bgTexture = Jam::Infrastructure::TextureLoader::getTexture(textureName);
			if (bgTexture) {
				// パララックス効果付きで背景を描画（より遠くに見えるように調整）
				const Vec2 parallaxOffset = cameraOffset * 0.1; // Back layer multiplier (0.3 → 0.1でよりゆっくり)
				const Vec2 drawPos(position.x - parallaxOffset.x, position.y - parallaxOffset.y);

				// テクスチャを指定サイズにリサイズして描画
				bgTexture->resized(size).draw(drawPos);
			}
			else
			{
				Print << U"[BackgroundRenderer] ❌ " << textureName << U" texture not found, using fallback";
			}
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
			}
			else {
				texture->resized(drawRect.size).drawAt(drawRect.center());
			}

			// リピート描画の場合の処理
			if (bgObj.isRepeating) {
				drawRepeatingTexture(*texture, bgObj, offset);
			}
		}

		// ループテクスチャ描画
		void drawRepeatingTexture(const Texture& texture, const Jam::Domain::Background::BackgroundObject& bgObj, const Vec2& offset) const {
			const RectF originalRect = bgObj.rect.movedBy(-offset);
			const Size textureSize = texture.size();
			
			// 画面外のマージンを追加（スクロール時に空白が見えないようにする）
			const double margin = 200.0;
			const RectF viewArea(
				originalRect.x - margin, originalRect.y - margin,
				originalRect.w + margin * 2, originalRect.h + margin * 2
			);
			
			// 水平方向の繰り返し
			if (bgObj.repeatMode == U"horizontal" || bgObj.repeatMode == U"both") {
				const double tileWidth = textureSize.x;
				const int startTileX = static_cast<int>(Math::Floor(viewArea.x / tileWidth));
				const int endTileX = static_cast<int>(Math::Ceil((viewArea.x + viewArea.w) / tileWidth));
				
				for (int x = startTileX; x <= endTileX; ++x) {
					const double drawX = x * tileWidth;
					const RectF tileRect(drawX, originalRect.y, tileWidth, originalRect.h);
					
					// 透明度を適用して描画
					if (bgObj.opacity < 1.0) {
						texture.resized(tileRect.size).draw(tileRect.pos, ColorF(1.0, bgObj.opacity));
					} else {
						texture.resized(tileRect.size).draw(tileRect.pos);
					}
				}
			}
			// 垂直方向の繰り返し（必要に応じて実装）
			else if (bgObj.repeatMode == U"vertical") {
				const double tileHeight = textureSize.y;
				const int startTileY = static_cast<int>(Math::Floor(viewArea.y / tileHeight));
				const int endTileY = static_cast<int>(Math::Ceil((viewArea.y + viewArea.h) / tileHeight));
				
				for (int y = startTileY; y <= endTileY; ++y) {
					const double drawY = y * tileHeight;
					const RectF tileRect(originalRect.x, drawY, originalRect.w, tileHeight);
					
					// 透明度を適用して描画  
					if (bgObj.opacity < 1.0) {
						texture.resized(tileRect.size).draw(tileRect.pos, ColorF(1.0, bgObj.opacity));
					} else {
						texture.resized(tileRect.size).draw(tileRect.pos);
					}
				}
			}
		}
	};
}
