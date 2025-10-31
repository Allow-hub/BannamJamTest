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
		
		// 背景画像の境界線を消すためのオーバーラップ量（ピクセル）
		static constexpr float OVERLAP_OFFSET = 1.f;

	public:
		BackgroundRenderer() = default;

		// 背景オブジェクトを設定
		void setBackgroundObjects(const Array<Jam::Domain::Background::BackgroundObject>& objects) {
			m_backgroundObjects = objects;
			generateBackgroundInstances();
			m_isLoaded = true;
		}

		// 指定されたレイヤーの背景を描画
		void drawLayer(Jam::Domain::Background::ParallaxLayer layer, const Vec2& cameraOffset) const {
			if (!m_isLoaded) return;

			const double parallaxMultiplier = Jam::Domain::Background::getParallaxMultiplier(layer);
			
			for (const auto& instance : m_backgroundInstances) {
				if (instance.layer == layer) {
					// パララックス効果を適用した位置を計算
					Vec2 parallaxPosition = instance.basePosition - (cameraOffset * parallaxMultiplier);
					
					// テクスチャを取得して描画
					const auto texture = Jam::Infrastructure::TextureLoader::getTexture(instance.textureName);
					if (texture) {
						// アスペクト比を保持してスケール
						const Size originalSize = texture->size();
						const double aspectRatio = static_cast<double>(originalSize.x) / originalSize.y;
						const double targetHeight = instance.rect.h;
						const double targetWidth = targetHeight * aspectRatio;
						const Size scaledSize(static_cast<int32>(targetWidth), static_cast<int32>(targetHeight));
						
						// 位置を整数ピクセルに丸めて境界線の問題を軽減
						const Vec2 roundedPosition = parallaxPosition.asPoint();
						
						if (instance.opacity < 1.0) {
							texture->resized(scaledSize).draw(roundedPosition, ColorF(1.0, instance.opacity));
						} else {
							texture->resized(scaledSize).draw(roundedPosition);
						}
					}
				}
			}
		}

		// 読み込み状態の確認
		bool isLoaded() const { return m_isLoaded; }

		// 背景オブジェクト数の取得
		size_t getObjectCount() const { return m_backgroundObjects.size(); }

	private:
		// 背景インスタンスを生成
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
				const double calculatedWidth = bgHeight * originalAspectRatio;
				const int32 bgWidth = static_cast<int32>(Math::Round(calculatedWidth));
				
				// JSONでwidthが0の場合は自動計算、それ以外は指定された範囲を使用
				double coverageWidth;
				double startX;
				
				if (bgObj.rect.w == 0) {
					// 自動計算: leftExtension/rightExtensionを使用
					coverageWidth = bgObj.leftExtension + bgObj.rightExtension;
					startX = bgObj.rect.x - bgObj.leftExtension;
				} else {
					// 明示的に指定された範囲を使用
					coverageWidth = bgObj.rect.w;
					startX = bgObj.rect.x;
				}
				
				// 必要なインスタンス数を計算
				const int instanceCount = static_cast<int>(Math::Ceil(coverageWidth / (bgWidth - OVERLAP_OFFSET))) + 2;
				
				// インスタンスを横に並べて生成
				for (int i = 0; i < instanceCount; ++i) {
					BackgroundInstance instance;
					const double xOffset = (bgWidth - OVERLAP_OFFSET) * i;
					instance.basePosition = Vec2(startX + xOffset, bgObj.rect.y);
					instance.rect = RectF(instance.basePosition, Size(bgWidth, static_cast<int32>(bgHeight)));
					instance.textureName = bgObj.textureName;
					instance.opacity = bgObj.opacity;
					instance.layer = bgObj.layer;
					
					m_backgroundInstances.push_back(instance);
				}
			}
		}
	};
}
