#pragma once
#include <Siv3D.hpp>
#include "Domain/Stage/BackgroundTypes.h"
#include "Infrastructure/TextureLoader.h"

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
		void setBackgroundObjects(const Array<Jam::Domain::Background::BackgroundObject>& objects);

		// 指定されたレイヤーの背景を描画
		void drawLayer(Jam::Domain::Background::ParallaxLayer layer, const Vec2& cameraOffset) const;

		// 読み込み状態の確認
		bool isLoaded() const;

		// 背景オブジェクト数の取得
		size_t getObjectCount() const;

	private:
		// 背景インスタンスを生成
		void generateBackgroundInstances();
	};
}
