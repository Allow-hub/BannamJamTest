#pragma once
#include <Siv3D.hpp>
#include "../../Domain/Stage/BackgroundTypes.h"

/**
 * 背景データの読み込みクラス
 * JSONファイルから背景オブジェクトを読み込む
 */
namespace Jam::Infrastructure::Background {
    
    class BackgroundLoader {
    public:
        // JSONファイルから背景データを読み込み
        static bool loadBackgroundFromFile(const String& fileName, Array<Jam::Domain::Background::BackgroundObject>& outObjects) {
            // 現在の作業ディレクトリを確認
            Print << U"[BackgroundLoader] Current working directory: " << FileSystem::CurrentDirectory();
            Print << U"[BackgroundLoader] Requested filename: " << fileName;
            
            String actualFilePath;
            Array<String> possiblePaths = {
                U"../Assets/Stage/" + fileName,
                U"Assets/Stage/" + fileName,
                U"../../../Assets/Stage/" + fileName,
                U"c:/Siv3D/BananaJam/Jam/Assets/Stage/" + fileName
            };
            
            bool found = false;
            for (const auto& path : possiblePaths) {
                Print << U"[BackgroundLoader] Trying path: " << path;
                if (FileSystem::Exists(path)) {
                    actualFilePath = path;
                    Print << U"[BackgroundLoader] ✅ Found at: " << path;
                    found = true;
                    break;
                } else {
                    Print << U"[BackgroundLoader] ❌ Not found at: " << path;
                }
            }
            
            if (!found) {
                Print << U"[BackgroundLoader] ❌ File not found at any path";
                return false;
            }
            
            const JSON json = JSON::Load(actualFilePath);
            if (!json) {
                Print << U"[BackgroundLoader] ❌ Failed to parse JSON: " << actualFilePath;
                return false;
            }
            
            outObjects.clear();
            
            // "backgrounds"配列から背景オブジェクトを読み込み
            if (!json.hasElement(U"backgrounds")) {
                Print << U"[BackgroundLoader] ⚠️  No 'backgrounds' array found in: " << fileName;
                return true; // 空でも成功とする
            }
            
            const auto& backgroundsArray = json[U"backgrounds"];
            if (!backgroundsArray.isArray()) {
                Print << U"[BackgroundLoader] ❌ 'backgrounds' is not an array";
                return false;
            }
            
            for (const auto& bgJson : backgroundsArray.arrayView()) {
                Jam::Domain::Background::BackgroundObject bgObj;
                
                if (!parseBackgroundObject(bgJson, bgObj)) {
                    Print << U"[BackgroundLoader] ⚠️  Skipped invalid background object";
                    continue;
                }
                
                outObjects.push_back(bgObj);
            }
            
            Print << U"[BackgroundLoader] ✅ Loaded " << outObjects.size() << U" background objects from " << fileName;
            return true;
        }
        
    private:
        // 単一の背景オブジェクトをJSONからパース
        static bool parseBackgroundObject(const JSON& json, Jam::Domain::Background::BackgroundObject& outObj) {
            Print << U"[BackgroundLoader] Parsing background object...";
            
            // 必須フィールドのチェック
            if (!json.hasElement(U"rect")) {
                Print << U"[BackgroundLoader] ❌ Missing 'rect' field";
                return false;
            }
            if (!json.hasElement(U"textureName")) {
                Print << U"[BackgroundLoader] ❌ Missing 'textureName' field";
                return false;
            }
            
            Print << U"[BackgroundLoader] ✅ Required fields found";
            
            // 位置・サイズの読み込み
            const auto& rectJson = json[U"rect"];
            Print << U"[BackgroundLoader] Parsing rect array...";
            
            if (!rectJson.isArray()) {
                Print << U"[BackgroundLoader] ❌ 'rect' is not an array";
                return false;
            }
            
            if (rectJson.size() < 4) {
                Print << U"[BackgroundLoader] ❌ 'rect' array has insufficient elements: " << rectJson.size();
                return false;
            }
            
            try {
                outObj.rect = RectF(
                    rectJson[0].get<double>(),
                    rectJson[1].get<double>(),
                    rectJson[2].get<double>(),
                    rectJson[3].get<double>()
                );
                Print << U"[BackgroundLoader] ✅ Rect parsed successfully";
            } catch (const std::exception& e) {
                Print << U"[BackgroundLoader] ❌ Error parsing rect values";
                return false;
            }
            
            // テクスチャ名
            outObj.textureName = json[U"textureName"].getString();
            
            // レイヤー（オプション、デフォルトはBack）
            if (json.hasElement(U"layer")) {
                outObj.layer = Jam::Domain::Background::stringToParallaxLayer(json[U"layer"].getString());
            }
            
            // メタデータ（オプション）
            if (json.hasElement(U"metadata")) {
                outObj.metadata = json[U"metadata"].getString();
            }
            
            // スクロール速度（オプション）
            if (json.hasElement(U"scrollSpeed") && json[U"scrollSpeed"].isArray() && json[U"scrollSpeed"].size() >= 2) {
                const auto& scrollJson = json[U"scrollSpeed"];
                outObj.scrollSpeed = Vec2(
                    scrollJson[0].get<double>(),
                    scrollJson[1].get<double>()
                );
            }
            
            // リピート設定（オプション）
            if (json.hasElement(U"isRepeating")) {
                outObj.isRepeating = json[U"isRepeating"].get<bool>();
                Console << U"[BackgroundLoader] ✅ isRepeating: " << outObj.isRepeating;
            }
            
            // リピートモード（オプション）
            if (json.hasElement(U"repeatMode")) {
                outObj.repeatMode = json[U"repeatMode"].getString();
                Console << U"[BackgroundLoader] ✅ repeatMode: " << outObj.repeatMode;
            }
            
            // 透明度（オプション）
            if (json.hasElement(U"opacity")) {
                outObj.opacity = json[U"opacity"].get<double>();
            }
            
            return true;
        }
    };
}