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
        
        // JSON読み込み（エラーハンドリング強化版）
        static bool loadFromJson(const String& jsonPath, Array<StageObject>& outObjects) {
            // ファイル存在確認
            if (!FileSystem::Exists(jsonPath)) {
                Print << U"ステージファイルが見つかりません: " + jsonPath;
                return false;
            }
            
            // JSON読み込み
            const JSON json = JSON::Load(jsonPath);
            if (!json) {
                Print << U"JSONの解析に失敗しました: " + jsonPath;
                return false;
            }
            
            if (!json.hasElement(U"objects")) {
                Print << U"'objects' 要素が見つかりません: " + jsonPath;
                return false;
            }
            
            const auto objectsArray = json[U"objects"].arrayView();
            outObjects.clear();
            
            size_t successCount = 0;
            size_t totalCount = 0;
            
            for (const auto& objJson : objectsArray) {
                totalCount++;
                if (auto obj = parseStageObject(objJson)) {
                    outObjects << *obj;
                    successCount++;
                } else {
                    Print << U"オブジェクト " + ToString(totalCount) + U" の解析に失敗しました";
                }
            }
            
            Print << U"ステージオブジェクトの読み込み完了: " + ToString(successCount) + U"/" + ToString(totalCount);
            return !outObjects.empty();
        }

    private:
        // ステージオブジェクト解析
        static Optional<StageObject> parseStageObject(const JSON& objJson) {
            StageObject obj;
            
            // 矩形データ解析（必須項目）
            if (!parseRect(objJson, obj.rect)) {
                return none;
            }
            
            // 当たり判定タイプ解析
            obj.type = objJson.hasElement(U"type") 
                ? Jam::Domain::Stage::stringToCollisionType(objJson[U"type"].getString())
                : CollisionType::None;
            
            // 色情報解析
            obj.color = objJson.hasElement(U"color") 
                ? parseColor(objJson[U"color"].getString())
                : Palette::Gray;
            
            // メタデータ解析
            obj.metadata = objJson.hasElement(U"metadata") 
                ? objJson[U"metadata"].getString()
                : U"";
            
            // 破壊可能フラグ解析
            obj.destructible = objJson.hasElement(U"destructible") 
                ? objJson[U"destructible"].get<bool>()
                : false;
            
            return obj;
        }
        
        // 矩形配列解析（配列形式[x,y,width,height]）
        static bool parseRect(const JSON& objJson, RectF& rect) {
            if (!objJson.hasElement(U"rect") || !objJson[U"rect"].isArray()) {
                return false;
            }
            
            const auto rectArray = objJson[U"rect"].arrayView();
            
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
        
        // 16進数カラーコード解析（例: "#FF0000"）
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