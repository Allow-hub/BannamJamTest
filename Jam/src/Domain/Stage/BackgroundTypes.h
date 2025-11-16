#pragma once
#include <Siv3D.hpp>

/**
 * 閭梧勹繧ｷ繧ｹ繝・Β髢｢騾｣縺ｮ蝙句ｮ夂ｾｩ
 * 繝代Λ繝ｩ繝・け繧ｹ閭梧勹謠冗判逕ｨ
 */
namespace Jam::Domain::Background {
    
    // 繝代Λ繝ｩ繝・け繧ｹ繝ｬ繧､繝､繝ｼ螳夂ｾｩ
    enum class ParallaxLayer {
        Back = 0,    // 荳逡ｪ蠕後ｍ・磯□譎ｯ縲・≦縺・虚縺搾ｼ・
        Middle = 1,  // 荳ｭ髢薙Ξ繧､繝､繝ｼ・井ｸｭ譎ｯ・・ 
        Front = 2    // 荳逡ｪ謇句燕・郁ｿ第勹縲・溘＞蜍輔″・・
    };

    // 閭梧勹繧ｪ繝悶ず繧ｧ繧ｯ繝域ｧ矩菴・
    struct BackgroundObject {
        RectF rect;                     // 菴咲ｽｮ繝ｻ繧ｵ繧､繧ｺ
        ParallaxLayer layer;            // 繝代Λ繝ｩ繝・け繧ｹ繝ｬ繧､繝､繝ｼ
        String textureName;             // 繝・け繧ｹ繝√Ε蜷・
        String metadata;                // 隴伜挨逕ｨID
        double opacity = 1.0;           // 騾乗・蠎ｦ
        double leftExtension = 3000.0;  // 蟾ｦ蛛ｴ縺ｸ縺ｮ諡｡蠑ｵ霍晞屬・医ョ繝輔か繝ｫ繝・000px・・
        double rightExtension = 17000.0;// 蜿ｳ蛛ｴ縺ｸ縺ｮ諡｡蠑ｵ霍晞屬・医ョ繝輔か繝ｫ繝・7000px・・
        
        // 繝・ヵ繧ｩ繝ｫ繝医さ繝ｳ繧ｹ繝医Λ繧ｯ繧ｿ
        BackgroundObject() 
            : rect(0, 0, 0, 0)
            , layer(ParallaxLayer::Back)
            , textureName(U"")
            , metadata(U"") {}
    };

    // 繝代Λ繝ｩ繝・け繧ｹ繝ｬ繧､繝､繝ｼ髢｢騾｣縺ｮ螟画鋤繝ｻ蜿門ｾ鈴未謨ｰ
    inline ParallaxLayer stringToParallaxLayer(const String& layerStr) {
        if (layerStr == U"back") return ParallaxLayer::Back;
        if (layerStr == U"middle") return ParallaxLayer::Middle;
        if (layerStr == U"front") return ParallaxLayer::Front;
        return ParallaxLayer::Back; // 繝・ヵ繧ｩ繝ｫ繝医・閭梧勹繝ｬ繧､繝､繝ｼ
    }



    // 繝代Λ繝ｩ繝・け繧ｹ騾溷ｺｦ蛟咲紫
    inline double getParallaxMultiplier(ParallaxLayer layer) {
        switch (layer) {
        case ParallaxLayer::Back: return 0.01; // 逕ｻ蜒上′縺ｧ縺肴ｬ｡隨ｬ縺薙％縺ｮ蛟､繧定ｪｿ謨ｴ
        case ParallaxLayer::Middle: return 0.05; // 荳願ｨ倥→蜷梧ｧ・
        case ParallaxLayer::Front: return 1.0;
        default: return 0.02;
        }
    }


}
