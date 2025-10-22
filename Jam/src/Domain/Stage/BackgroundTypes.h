#pragma once
#include <Siv3D.hpp>

/**
 * 背景システム関連の型定義
 * パララックス背景描画用
 */
namespace Jam::Domain::Background {
    
    // パララックスレイヤー定義
    enum class ParallaxLayer {
        Back = 0,    // 一番後ろ（遠景、遅い動き）
        Middle = 1,  // 中間レイヤー（中景）  
        Front = 2    // 一番手前（近景、速い動き）
    };

    // 背景オブジェクト構造体
    struct BackgroundObject {
        RectF rect;                     // 位置・サイズ
        ParallaxLayer layer;            // パララックスレイヤー
        String textureName;             // テクスチャ名
        String metadata;                // 識別用ID
        double opacity = 1.0;           // 透明度
        
        // デフォルトコンストラクタ
        BackgroundObject() 
            : rect(0, 0, 0, 0)
            , layer(ParallaxLayer::Back)
            , textureName(U"")
            , metadata(U"") {}
    };

    // パララックスレイヤー関連の変換・取得関数
    inline ParallaxLayer stringToParallaxLayer(const String& layerStr) {
        if (layerStr == U"back") return ParallaxLayer::Back;
        if (layerStr == U"middle") return ParallaxLayer::Middle;
        if (layerStr == U"front") return ParallaxLayer::Front;
        return ParallaxLayer::Back; // デフォルトは背景レイヤー
    }



    // パララックス速度倍率（Playerが基準速度1.0）
    inline double getParallaxMultiplier(ParallaxLayer layer) {
        switch (layer) {
        case ParallaxLayer::Back: return 0.2;    // 背景は遅く（20%）
        case ParallaxLayer::Middle: return 0.6;  // 中間（60%）
        case ParallaxLayer::Front: return 0.9;   // 前景はほぼ同じ速度（90%）
        default: return 0.3;
        }
    }


}
