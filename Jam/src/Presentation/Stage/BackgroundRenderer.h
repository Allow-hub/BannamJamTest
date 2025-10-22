#pragma once
#include <Siv3D.hpp>
#include "../../Domain/Stage/BackgroundTypes.h"
#include "../../Infrastructure/TextureLoader.h"

/**
 * 背景描画クラス
 * パララックス効果付きの背景レンダリング
 */
namespace Jam::Presentation::Background {

	// 背景インスタンス（個別の背景画像の情報）
	struct BackgroundInstance {
		Vec2 position;          // 現在の位置
		Vec2 basePosition;      // 基準位置
		RectF rect;             // 描画領域
		String textureName;     // テクスチャ名
		double opacity;         // 透明度
		Jam::Domain::Background::ParallaxLayer layer; // レイヤー
	};

	class BackgroundRenderer {
	private:
		Array<Jam::Domain::Background::BackgroundObject> m_backgroundObjects;
		Array<BackgroundInstance> m_backgroundInstances; // 生成された背景インスタンス群
		bool m_isLoaded = false;

	public:
		BackgroundRenderer() = default;

		// 背景オブジェクトを設定
		void setBackgroundObjects(const Array<Jam::Domain::Background::BackgroundObject>& objects) {
			m_backgroundObjects = objects;
			
			// 背景インスタンスを生成
			generateBackgroundInstances();
			
			m_isLoaded = true;
			Console << U"[BackgroundRenderer] ✅ Set " << objects.size() << U" background objects, generated " << m_backgroundInstances.size() << U" instances";
		}

		// 指定されたレイヤーの背景を描画（新方式：インスタンスベース）
		void drawLayer(Jam::Domain::Background::ParallaxLayer layer, const Vec2& cameraOffset) const {
			if (!m_isLoaded) return;

			const double parallaxMultiplier = Jam::Domain::Background::getParallaxMultiplier(layer);
			
			// デバッグ出力（1秒間隔で制限）
			static double lastDebugTime = 0.0;
			if (Scene::Time() - lastDebugTime > 1.0) {
				Console << U"[BackgroundRenderer] Camera: (" << cameraOffset.x << U", " << cameraOffset.y << U"), Parallax: " << parallaxMultiplier;
				lastDebugTime = Scene::Time();
			}
			
			for (const auto& instance : m_backgroundInstances) {
				if (instance.layer == layer) {
					// パララックス効果を適用した位置を計算（逆方向移動）
					Vec2 parallaxPosition = instance.basePosition - (cameraOffset * parallaxMultiplier);
					
					// テクスチャを取得して描画
					const auto texture = Jam::Infrastructure::TextureLoader::getTexture(instance.textureName);
					if (texture) {
						// テクスチャの元のアスペクト比を保持
						const Size originalSize = texture->size();
						const double originalAspectRatio = static_cast<double>(originalSize.x) / originalSize.y;
						
						// 指定された高さに合わせて幅を調整（アスペクト比保持）
						const double targetHeight = instance.rect.h;
						const double targetWidth = targetHeight * originalAspectRatio;
						const Size scaledSize(static_cast<int32>(targetWidth), static_cast<int32>(targetHeight));
						
						if (instance.opacity < 1.0) {
							texture->resized(scaledSize).draw(parallaxPosition, ColorF(1.0, instance.opacity));
						} else {
							texture->resized(scaledSize).draw(parallaxPosition);
						}
					}
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
				//Print << U"[BackgroundRenderer] ❌ " << textureName << U" texture not found, using fallback";
			}
		}

		// 読み込み状態の確認
		bool isLoaded() const { return m_isLoaded; }

		// 背景オブジェクト数の取得
		size_t getObjectCount() const { return m_backgroundObjects.size(); }

	private:
		// 背景インスタンスを生成（初期化時に複数の背景を横に配置）
		void generateBackgroundInstances() {
			m_backgroundInstances.clear();
			
			for (const auto& bgObj : m_backgroundObjects) {
				// テクスチャを取得してアスペクト比を計算
				const auto texture = Jam::Infrastructure::TextureLoader::getTexture(bgObj.textureName);
				if (!texture) continue;
				
				const Size originalSize = texture->size();
				const double originalAspectRatio = static_cast<double>(originalSize.x) / originalSize.y;
				
				// 高さに基づいてアスペクト比を保持した幅を計算
				const double bgHeight = bgObj.rect.h;
				const double bgWidth = bgHeight * originalAspectRatio;
				
				// 画面幅を考慮して必要な背景数を計算
				const double screenWidth = Scene::Width();
				const int instanceCount = static_cast<int>(Math::Ceil((screenWidth * 3) / bgWidth)) + 2; // 余裕を持たせる
				
				Console << U"[BackgroundRenderer] Generating " << instanceCount << U" instances for " << bgObj.textureName << U" (aspect ratio preserved width: " << bgWidth << U")";
				
				// 背景インスタンスを横に並べて生成
				for (int i = 0; i < instanceCount; ++i) {
					BackgroundInstance instance;
					instance.basePosition = Vec2(bgObj.rect.x + (i * bgWidth), bgObj.rect.y);
					instance.position = instance.basePosition;
					instance.rect = RectF(instance.basePosition, Size(static_cast<int32>(bgWidth), static_cast<int32>(bgHeight)));
					instance.textureName = bgObj.textureName;
					instance.opacity = bgObj.opacity;
					instance.layer = bgObj.layer;
					
					m_backgroundInstances.push_back(instance);
				}
			}
		}

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

			// リピート描画の場合の処理
			if (bgObj.isRepeating) {
				Console << U"[BackgroundRenderer] Drawing repeating texture for: " << bgObj.textureName << U", repeatMode: " << bgObj.repeatMode;
				drawRepeatingTexture(*texture, bgObj, offset);
			}
			else {
				// 通常の単体描画
				const RectF drawRect = bgObj.rect.movedBy(-offset);

				// 透明度を適用して描画
				if (bgObj.opacity < 1.0) {
					texture->resized(drawRect.size).drawAt(drawRect.center(), ColorF(1.0, bgObj.opacity));
				}
				else {
					texture->resized(drawRect.size).drawAt(drawRect.center());
				}
			}
		}

		// ループテクスチャ描画
		void drawRepeatingTexture(const Texture& texture, const Jam::Domain::Background::BackgroundObject& bgObj, const Vec2& offset) const {
			const RectF originalRect = bgObj.rect.movedBy(-offset);
			
			Console << U"[BackgroundRenderer] Repeating texture - originalRect: " << originalRect << U", offset: " << offset;
			
			// 画面の可視範囲を大きめに設定（スクロール時に空白が見えないように）
			const double margin = 1000.0; // マージンを大きくして確実にカバー
			const RectF screenArea = Scene::Rect().stretched(margin);
			
			Console << U"[BackgroundRenderer] Screen area with margin: " << screenArea;
			
			// 水平方向の繰り返し
			if (bgObj.repeatMode == U"horizontal" || bgObj.repeatMode == U"both") {
				// テクスチャの元のアスペクト比を保持してタイル幅を計算
				const Size originalTextureSize = texture.size();
				const double originalAspectRatio = static_cast<double>(originalTextureSize.x) / originalTextureSize.y;
				const double tileHeight = originalRect.h;
				const double tileWidth = tileHeight * originalAspectRatio; // アスペクト比保持
				
				// 画面領域をカバーするのに必要なタイル数を計算
				const int startTileX = static_cast<int>(Math::Floor((screenArea.x - originalRect.x) / tileWidth));
				const int endTileX = static_cast<int>(Math::Ceil((screenArea.x + screenArea.w - originalRect.x) / tileWidth));
				
				Console << U"[BackgroundRenderer] Horizontal repeat - tileWidth: " << tileWidth 
					  << U", startTileX: " << startTileX << U", endTileX: " << endTileX;
				
				for (int x = startTileX; x <= endTileX; ++x) {
					const double drawX = originalRect.x + (x * tileWidth);
					const Size scaledSize(static_cast<int32>(tileWidth), static_cast<int32>(tileHeight));
					
					Console << U"[BackgroundRenderer] Drawing tile " << x << U" at: (" << drawX << U", " << originalRect.y << U")";
					
					// 透明度を適用して描画（アスペクト比保持）
					if (bgObj.opacity < 1.0) {
						texture.resized(scaledSize).draw(Vec2(drawX, originalRect.y), ColorF(1.0, bgObj.opacity));
					} else {
						texture.resized(scaledSize).draw(Vec2(drawX, originalRect.y));
					}
				}
			}
			// 垂直方向の繰り返し（必要に応じて実装）
			else if (bgObj.repeatMode == U"vertical") {
				const double tileHeight = bgObj.rect.h; // 元のJSONサイズを使用
				
				// 画面領域をカバーするのに必要なタイル数を計算
				const int startTileY = static_cast<int>(Math::Floor((screenArea.y - originalRect.y) / tileHeight));
				const int endTileY = static_cast<int>(Math::Ceil((screenArea.y + screenArea.h - originalRect.y) / tileHeight));
				
				Console << U"[BackgroundRenderer] Vertical repeat - tileHeight: " << tileHeight 
					  << U", startTileY: " << startTileY << U", endTileY: " << endTileY;
				
				for (int y = startTileY; y <= endTileY; ++y) {
					const double drawY = originalRect.y + (y * tileHeight);
					const RectF tileRect(originalRect.x, drawY, originalRect.w, tileHeight);
					
					Console << U"[BackgroundRenderer] Drawing tile " << y << U" at: " << tileRect;
					
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
