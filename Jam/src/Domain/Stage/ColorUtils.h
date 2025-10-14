#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Stage {
    
    // 色変換ユーティリティクラス
    // ドメイン層で色に関するビジネスルールを管理
    class ColorUtils {
    public:
        // 文字列から色を解析（16進数またはSiv3D色名）
        static Color parseColorString(const String& colorStr) {
            // 16進数カラーコード（例: "#FF0000"）
            if (colorStr.starts_with(U'#') && colorStr.length() == 7) {
                return parseHexColor(colorStr);
            }
            
            // Siv3D Palette直接マッピング
            return getPaletteColor(colorStr);
        }
        
        // ゲーム用途別の推奨色を取得
        static Color getGameColor(const String& purpose) {
            const String lowerPurpose = purpose.lowercased();
            
            if (lowerPurpose == U"ground") return Palette::Saddlebrown;
            if (lowerPurpose == U"platform") return Palette::Brown;
            if (lowerPurpose == U"wall") return Palette::Gray;
            if (lowerPurpose == U"breakable") return Palette::Orangered;
            if (lowerPurpose == U"hazard") return Palette::Crimson;
            if (lowerPurpose == U"trigger") return Palette::Cyan;
            if (lowerPurpose == U"collectible") return Palette::Gold;
            
            return Palette::Gray;
        }
        
    private:
        // 16進数カラーコード解析
        static Color parseHexColor(const String& hexStr) {
            try {
                const String hex = hexStr.substr(1);
                const uint32 colorValue = ParseInt<uint32>(hex, 16);
                return Color(
                    static_cast<uint8>((colorValue >> 16) & 0xFF),
                    static_cast<uint8>((colorValue >> 8) & 0xFF),
                    static_cast<uint8>(colorValue & 0xFF)
                );
            } catch (...) {
                return Palette::Gray;
            }
        }
        
        // Siv3D Palette直接マッピング（ハッシュマップ使用）
        static Color getPaletteColor(const String& colorName) {
            static const HashTable<String, Color> paletteMap = {
                // 基本色
                {U"aliceblue", Palette::Aliceblue},
                {U"antiquewhite", Palette::Antiquewhite},
                {U"aqua", Palette::Aqua},
                {U"aquamarine", Palette::Aquamarine},
                {U"azure", Palette::Azure},
                {U"beige", Palette::Beige},
                {U"bisque", Palette::Bisque},
                {U"black", Palette::Black},
                {U"blanchedalmond", Palette::Blanchedalmond},
                {U"blue", Palette::Blue},
                {U"blueviolet", Palette::Blueviolet},
                {U"brown", Palette::Brown},
                {U"burlywood", Palette::Burlywood},
                {U"cadetblue", Palette::Cadetblue},
                {U"chartreuse", Palette::Chartreuse},
                {U"chocolate", Palette::Chocolate},
                {U"coral", Palette::Coral},
                {U"cornflowerblue", Palette::Cornflowerblue},
                {U"cornsilk", Palette::Cornsilk},
                {U"crimson", Palette::Crimson},
                {U"cyan", Palette::Cyan},
                {U"darkblue", Palette::Darkblue},
                {U"darkcyan", Palette::Darkcyan},
                {U"darkgoldenrod", Palette::Darkgoldenrod},
                {U"darkgray", Palette::Darkgray},
                {U"darkgreen", Palette::Darkgreen},
                {U"darkgrey", Palette::Darkgrey},
                {U"darkkhaki", Palette::Darkkhaki},
                {U"darkmagenta", Palette::Darkmagenta},
                {U"darkolivegreen", Palette::Darkolivegreen},
                {U"darkorange", Palette::Darkorange},
                {U"darkorchid", Palette::Darkorchid},
                {U"darkred", Palette::Darkred},
                {U"darksalmon", Palette::Darksalmon},
                {U"darkseagreen", Palette::Darkseagreen},
                {U"darkslateblue", Palette::Darkslateblue},
                {U"darkslategray", Palette::Darkslategray},
                {U"darkslategrey", Palette::Darkslategrey},
                {U"darkturquoise", Palette::Darkturquoise},
                {U"darkviolet", Palette::Darkviolet},
                {U"deeppink", Palette::Deeppink},
                {U"deepskyblue", Palette::Deepskyblue},
                {U"dimgray", Palette::Dimgray},
                {U"dimgrey", Palette::Dimgrey},
                {U"dodgerblue", Palette::Dodgerblue},
                {U"firebrick", Palette::Firebrick},
                {U"floralwhite", Palette::Floralwhite},
                {U"forestgreen", Palette::Forestgreen},
                {U"fuchsia", Palette::Fuchsia},
                {U"gainsboro", Palette::Gainsboro},
                {U"ghostwhite", Palette::Ghostwhite},
                {U"gold", Palette::Gold},
                {U"goldenrod", Palette::Goldenrod},
                {U"gray", Palette::Gray},
                {U"green", Palette::Green},
                {U"greenyellow", Palette::Greenyellow},
                {U"grey", Palette::Grey},
                {U"honeydew", Palette::Honeydew},
                {U"hotpink", Palette::Hotpink},
                {U"indianred", Palette::Indianred},
                {U"indigo", Palette::Indigo},
                {U"ivory", Palette::Ivory},
                {U"khaki", Palette::Khaki},
                {U"lavender", Palette::Lavender},
                {U"lavenderblush", Palette::Lavenderblush},
                {U"lawngreen", Palette::Lawngreen},
                {U"lemonchiffon", Palette::Lemonchiffon},
                {U"lightblue", Palette::Lightblue},
                {U"lightcoral", Palette::Lightcoral},
                {U"lightcyan", Palette::Lightcyan},
                {U"lightgoldenrodyellow", Palette::Lightgoldenrodyellow},
                {U"lightgray", Palette::Lightgray},
                {U"lightgreen", Palette::Lightgreen},
                {U"lightgrey", Palette::Lightgrey},
                {U"lightpink", Palette::Lightpink},
                {U"lightsalmon", Palette::Lightsalmon},
                {U"lightseagreen", Palette::Lightseagreen},
                {U"lightskyblue", Palette::Lightskyblue},
                {U"lightslategray", Palette::Lightslategray},
                {U"lightslategrey", Palette::Lightslategrey},
                {U"lightsteelblue", Palette::Lightsteelblue},
                {U"lightyellow", Palette::Lightyellow},
                {U"lime", Palette::Lime},
                {U"limegreen", Palette::Limegreen},
                {U"linen", Palette::Linen},
                {U"magenta", Palette::Magenta},
                {U"maroon", Palette::Maroon},
                {U"mediumaquamarine", Palette::Mediumaquamarine},
                {U"mediumblue", Palette::Mediumblue},
                {U"mediumorchid", Palette::Mediumorchid},
                {U"mediumpurple", Palette::Mediumpurple},
                {U"mediumseagreen", Palette::Mediumseagreen},
                {U"mediumslateblue", Palette::Mediumslateblue},
                {U"mediumspringgreen", Palette::Mediumspringgreen},
                {U"mediumturquoise", Palette::Mediumturquoise},
                {U"mediumvioletred", Palette::Mediumvioletred},
                {U"midnightblue", Palette::Midnightblue},
                {U"mintcream", Palette::Mintcream},
                {U"mistyrose", Palette::Mistyrose},
                {U"moccasin", Palette::Moccasin},
                {U"navajowhite", Palette::Navajowhite},
                {U"navy", Palette::Navy},
                {U"oldlace", Palette::Oldlace},
                {U"olive", Palette::Olive},
                {U"olivedrab", Palette::Olivedrab},
                {U"orange", Palette::Orange},
                {U"orangered", Palette::Orangered},
                {U"orchid", Palette::Orchid},
                {U"palegoldenrod", Palette::Palegoldenrod},
                {U"palegreen", Palette::Palegreen},
                {U"paleturquoise", Palette::Paleturquoise},
                {U"palevioletred", Palette::Palevioletred},
                {U"papayawhip", Palette::Papayawhip},
                {U"peachpuff", Palette::Peachpuff},
                {U"peru", Palette::Peru},
                {U"pink", Palette::Pink},
                {U"plum", Palette::Plum},
                {U"powderblue", Palette::Powderblue},
                {U"purple", Palette::Purple},
                {U"red", Palette::Red},
                {U"rosybrown", Palette::Rosybrown},
                {U"royalblue", Palette::Royalblue},
                {U"saddlebrown", Palette::Saddlebrown},
                {U"salmon", Palette::Salmon},
                {U"sandybrown", Palette::Sandybrown},
                {U"seagreen", Palette::Seagreen},
                {U"seashell", Palette::Seashell},
                {U"sienna", Palette::Sienna},
                {U"silver", Palette::Silver},
                {U"skyblue", Palette::Skyblue},
                {U"slateblue", Palette::Slateblue},
                {U"slategray", Palette::Slategray},
                {U"slategrey", Palette::Slategrey},
                {U"snow", Palette::Snow},
                {U"springgreen", Palette::Springgreen},
                {U"steelblue", Palette::Steelblue},
                {U"tan", Palette::Tan},
                {U"teal", Palette::Teal},
                {U"thistle", Palette::Thistle},
                {U"tomato", Palette::Tomato},
                {U"turquoise", Palette::Turquoise},
                {U"violet", Palette::Violet},
                {U"wheat", Palette::Wheat},
                {U"white", Palette::White},
                {U"whitesmoke", Palette::Whitesmoke},
                {U"yellow", Palette::Yellow},
                {U"yellowgreen", Palette::Yellowgreen}
            };
            
            const String lowerColorName = colorName.lowercased();
            if (paletteMap.contains(lowerColorName)) {
                return paletteMap.at(lowerColorName);
            }
            
            return Palette::Gray; // デフォルト
        }
    };
}