#include "JsonStageLoader.h"

namespace Jam::Infrastructure {
    
    bool JsonStageLoader::loadStageData(const String& path, Domain::Stage::StageInfo& outInfo) {
        // JSONファイルの読み込み
        JSON json = JSON::Load(path);
        if (!json) {
            Print << U"Failed to load JSON: " << path;
            return false;
        }
        
        // ステージ情報の設定
        outInfo.id = FileSystem::BaseName(path);
        
        if (json.hasElement(U"name")) {
            outInfo.name = json[U"name"].getString();
        } else {
            outInfo.name = U"Unknown";
        }
        
        // オブジェクトの読み込み
        if (json.hasElement(U"objects")) {
            outInfo.objects = parseObjects(json[U"objects"]);
        }
        
        Print << U"Stage loaded: " << outInfo.name << U" (Objects: " << outInfo.objects.size() << U")";
        return true;
    }
    
    Array<Domain::Stage::StageObject> JsonStageLoader::parseObjects(const JSON& objectsJson) {
        Array<Domain::Stage::StageObject> objects;
        
        if (!objectsJson.isArray()) {
            return objects;
        }
        
        for (const auto& objJson : objectsJson.arrayView()) {
            Domain::Stage::StageObject obj;
            
            // 矩形の読み込み（配列形式 [x, y, width, height] をサポート）
            if (objJson.hasElement(U"rect")) {
                const auto& rectJson = objJson[U"rect"];
                if (rectJson.isArray()) {
                    // 配列形式: [x, y, width, height]
                    const auto& arr = rectJson.arrayView();
                    Array<JSON> elements;
                    for (const auto& elem : arr) {
                        elements << elem;
                    }
                    if (elements.size() >= 4) {
                        obj.rect = RectF(
                            elements[0].get<double>(),
                            elements[1].get<double>(),
                            elements[2].get<double>(),
                            elements[3].get<double>()
                        );
                    }
                } else if (rectJson.isObject()) {
                    // オブジェクト形式（従来互換性のため）: {x, y, width, height}
                    obj.rect = RectF(
                        rectJson[U"x"].get<double>(),
                        rectJson[U"y"].get<double>(),
                        rectJson[U"width"].get<double>(),
                        rectJson[U"height"].get<double>()
                    );
                }
            }
            
            // タイプの読み込み
            if (objJson.hasElement(U"type")) {
                obj.type = stringToCollisionType(objJson[U"type"].getString());
            }
            
            // 色の読み込み
            if (objJson.hasElement(U"color")) {
                obj.color = parseColor(objJson[U"color"].getString());
            } else {
                obj.color = Palette::Gray; // デフォルト色
            }
            
            // メタデータの読み込み
            if (objJson.hasElement(U"metadata")) {
                obj.metadata = objJson[U"metadata"].getString();
            }
            
            // 破壊可能フラグの読み込み
            if (objJson.hasElement(U"destructible")) {
                obj.destructible = objJson[U"destructible"].get<bool>();
            }

            objects << obj;
        }
        
        return objects;
    }
    
    Domain::Stage::CollisionType JsonStageLoader::stringToCollisionType(const String& typeStr) {
        if (typeStr == U"solid") return Domain::Stage::CollisionType::Solid;
        if (typeStr == U"platform") return Domain::Stage::CollisionType::Platform;
        if (typeStr == U"hazard") return Domain::Stage::CollisionType::Hazard;
        if (typeStr == U"trigger") return Domain::Stage::CollisionType::Trigger;
        if (typeStr == U"breakable") return Domain::Stage::CollisionType::Breakable;
        return Domain::Stage::CollisionType::None;
    }
    
    Color JsonStageLoader::parseColor(const String& colorStr) {
        // HTMLカラーコード（#RRGGBB）の解析
        if (colorStr.starts_with(U'#') && colorStr.length() == 7) {
            try {
                String hexStr = colorStr.substr(1); // # を除去
                uint32 colorValue = ParseInt<uint32>(hexStr, 16);
                return Color(
                    (colorValue >> 16) & 0xFF, // R
                    (colorValue >> 8) & 0xFF,  // G
                    colorValue & 0xFF          // B
                );
            } catch (...) {
                return Palette::Gray;
            }
        }
        
        // デフォルト色
        return Palette::Gray;
    }
}