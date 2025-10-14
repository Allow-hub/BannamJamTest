#pragma once
#include <Siv3D.hpp>
#include "../Domain/Stage/StageTypes.h"

namespace Jam::Infrastructure::Stage {
    
    using StageObject = Jam::Domain::Stage::StageObject;
    using CollisionType = Jam::Domain::Stage::CollisionType;
    
    class StageLoader {
    private:
        // 定数定義
        static constexpr size_t RECT_ARRAY_SIZE = 4;
        static constexpr size_t HEX_COLOR_LENGTH = 7;
        
    public:
        // ステージファイル読み込み
        static bool loadStageFromFile(const String& stageFileName, Array<StageObject>& outObjects) {
            const String stagePath = U"Stage/" + stageFileName;
            return loadFromJson(stagePath, outObjects);
        }
        
        // JSON読み込み
        static bool loadFromJson(const String& jsonPath, Array<StageObject>& outObjects) {
            if (!FileSystem::Exists(jsonPath)) {
                return false;
            }
            
            const JSON json = JSON::Load(jsonPath);
            if (!json || !json.hasElement(U"objects")) {
                return false;
            }
            
            const auto objectsArray = json[U"objects"].arrayView();
            outObjects.clear();
            // reserve は配列サイズが分からないのでスキップ
            
            for (const auto& objJson : objectsArray) {
                if (auto obj = parseStageObject(objJson)) {
                    outObjects << *obj;
                }
            }
            
            return !outObjects.empty();
        }
        
    private:
        // ステージオブジェクト解析
        static Optional<StageObject> parseStageObject(const JSON& objJson) {
            StageObject obj;
            
            // rect解析（必須）
            if (!parseRect(objJson, obj.rect)) {
                return none;
            }
            
            // type解析
            obj.type = objJson.hasElement(U"type") 
                ? Jam::Domain::Stage::stringToCollisionType(objJson[U"type"].getString())
                : CollisionType::None;
            
            // color解析
            obj.color = objJson.hasElement(U"color") 
                ? parseColor(objJson[U"color"].getString())
                : Palette::Gray;
            
            // metadata解析
            obj.metadata = objJson.hasElement(U"metadata") 
                ? objJson[U"metadata"].getString()
                : U"";
            
            // destructible解析
            obj.destructible = objJson.hasElement(U"destructible") 
                ? objJson[U"destructible"].get<bool>()
                : false;
            
            return obj;
        }
        
        // rect配列解析（改善版）
        static bool parseRect(const JSON& objJson, RectF& rect) {
            if (!objJson.hasElement(U"rect") || !objJson[U"rect"].isArray()) {
                return false;
            }
            
            const auto rectArray = objJson[U"rect"].arrayView();
            
            // 直接アクセスで中間配列を回避
            try {
                double values[RECT_ARRAY_SIZE];
                size_t index = 0;
                
                for (const auto& elem : rectArray) {
                    if (index >= RECT_ARRAY_SIZE) break;
                    values[index++] = elem.get<double>();
                }
                
                if (index < RECT_ARRAY_SIZE) {
                    return false;
                }
                
                rect = RectF(values[0], values[1], values[2], values[3]);
                return true;
                
            } catch (...) {
                return false;
            }
        }
        
        // 色解析（改善版）
        static Color parseColor(const String& colorStr) {
            if (!colorStr.starts_with(U'#') || colorStr.length() != HEX_COLOR_LENGTH) {
                return Palette::Gray;
            }
            
            try {
                const String hexStr = colorStr.substr(1);
                const uint32 colorValue = ParseInt<uint32>(hexStr, 16);
                return Color(
                    static_cast<uint8>((colorValue >> 16) & 0xFF),
                    static_cast<uint8>((colorValue >> 8) & 0xFF),
                    static_cast<uint8>(colorValue & 0xFF)
                );
            } catch (...) {
                return Palette::Gray;
            }
        }
    };
}