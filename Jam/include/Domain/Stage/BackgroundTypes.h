#pragma once
#include <Siv3D.hpp>

/**
 * 背景システム関連の型定義
 * パララックス背景描画用
 */
namespace Jam::Domain::Background {
    
    // パララックスレイヤー定義
    enum class ParallaxLayer {
        Back = 0,    // 一番後ろ(遠景、遅い動き)
        Middle = 1,  // 中間レイヤー(中景) 
        Front = 2    // 一番手前(近景、速い動き)
    };

    // 背景オブジェクト構造
    struct BackgroundObject {
        RectF rect;                     // 位置・サイズ
        ParallaxLayer layer;            // パララックスレイヤー
        String textureName;             // テクスチャ名
        String metadata;                // 識別用ID
        double opacity = 1.0;           // 透明度
        double leftExtension = 3000.0;  // 左側への延長範囲(デフォルト3000px)
        double rightExtension = 17000.0;// 右側への延長範囲(デフォルト17000px)
        
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



    // パララックス速度計算
    inline double getParallaxMultiplier(ParallaxLayer layer) {
        switch (layer) {
        case ParallaxLayer::Back: return 0.01; // 画像が出る次第ここの値を調整
        case ParallaxLayer::Middle: return 0.05; // 上記と同様
        case ParallaxLayer::Front: return 1.0;
        default: return 0.02;
        }
    }


}
