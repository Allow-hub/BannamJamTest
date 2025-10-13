#include "Stage.h"

namespace Jam::Domain::Stage {
    
    Stage::Stage() : m_isLoaded(false) {
        // 初期化
    }
    
    bool Stage::loadFromJson(const String& jsonPath) {
        // JSONファイルの読み込み
        JSON json = JSON::Load(jsonPath);
        if (!json) {
            Print << U"Failed to load JSON: " << jsonPath;
            return false;
        }
        
        // ステージ情報の設定
        m_info.id = FileSystem::BaseName(jsonPath);
        
        if (json.hasElement(U"name")) {
            m_info.name = json[U"name"].getString();
        } else {
            m_info.name = U"Unknown";
        }
        
        // ステージサイズの読み込み
        if (json.hasElement(U"size")) {
            const auto& sizeJson = json[U"size"];
            m_info.size = Size(
                sizeJson[U"width"].get<int32>(),
                sizeJson[U"height"].get<int32>()
            );
        } else {
            m_info.size = Size(1280, 720); // デフォルトサイズ
        }
        
        // オブジェクトの読み込み
        if (json.hasElement(U"objects")) {
            m_info.objects = parseObjects(json[U"objects"]);
        }
        
        m_isLoaded = true;
        Print << U"Stage loaded: " << m_info.name << U" (Objects: " << m_info.objects.size() << U")";
        return true;
    }
    
    void Stage::draw() const {
        if (!m_isLoaded) {
            return;
        }
        
        // 全オブジェクトを描画
        for (const auto& obj : m_info.objects) {
            obj.rect.draw(obj.color);
        }
    }
    
    Array<StageObject> Stage::parseObjects(const JSON& objectsJson) {
        Array<StageObject> objects;
        
        if (!objectsJson.isArray()) {
            return objects;
        }
        
        for (const auto& objJson : objectsJson.arrayView()) {
            StageObject obj;
            
            // 矩形の読み込み
            if (objJson.hasElement(U"rect")) {
                const auto& rectJson = objJson[U"rect"];
                obj.rect = RectF(
                    rectJson[U"x"].get<double>(),
                    rectJson[U"y"].get<double>(),
                    rectJson[U"width"].get<double>(),
                    rectJson[U"height"].get<double>()
                );
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
            
            objects << obj;
        }
        
        return objects;
    }
    
    CollisionType Stage::stringToCollisionType(const String& typeStr) {
        if (typeStr == U"solid") return CollisionType::Solid;
        if (typeStr == U"platform") return CollisionType::Platform;
        if (typeStr == U"hazard") return CollisionType::Hazard;
        if (typeStr == U"trigger") return CollisionType::Trigger;
        return CollisionType::None;
    }
    
    Color Stage::parseColor(const String& colorStr) {
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