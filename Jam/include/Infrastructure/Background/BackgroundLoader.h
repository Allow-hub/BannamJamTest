#pragma once
#include <Siv3D.hpp>
#include "Domain/Stage/BackgroundTypes.h"
#include "Foundation/CoreManager.h"

/**
 * 背景データの読み込みクラス
 * JSONファイルから背景オブジェクトを読み込む
 */
namespace Jam::Infrastructure::Background {
    
	class BackgroundLoader {
	public:
		// JSONファイルから背景データを読み込み
		static bool loadBackgroundFromFile(const String& fileName, Jam::Foundation::StageName stageName, Array<Jam::Domain::Background::BackgroundObject>& outObjects) {
			// StageName enumを文字列に変換
			String stageNameStr;
			switch (stageName) {
			case Jam::Foundation::StageName::Stage1_1:
				stageNameStr = U"Stage1_1";
				break;
			case Jam::Foundation::StageName::Stage1_2:
				stageNameStr = U"Stage1_2";
				break;
			case Jam::Foundation::StageName::Stage1_3:
				stageNameStr = U"Stage1_3";
				break;
			default:
				stageNameStr = U"Stage1_3";
				break;
			}
			
			return loadBackgroundFromFile(fileName, stageNameStr, outObjects);
		}
		
		// JSONファイルから背景データを読み込み（文字列版）
		static bool loadBackgroundFromFile(const String& fileName, const String& stageName, Array<Jam::Domain::Background::BackgroundObject>& outObjects) {

			String actualFilePath;
			Array<String> possiblePaths = {
				U"../Assets/Stage/" + fileName,
				U"Assets/Stage/" + fileName,
				U"../../../Assets/Stage/" + fileName,
				U"c:/Siv3D/BananaJam/Jam/Assets/Stage/" + fileName
			};

			bool found = false;
			for (const auto& path : possiblePaths) {
				if (FileSystem::Exists(path)) {
					actualFilePath = path;
					found = true;
					break;
				}
			}

			if (!found) {
				return false;
			}

			const JSON json = JSON::Load(actualFilePath);
			if (!json) {
				return false;
			}

			outObjects.clear();

			// ステージ名で背景配列を取得
			if (!json.hasElement(stageName)) {
				Console << U"[BackgroundLoader] Stage '{}' not found in JSON"_fmt(stageName);
				return false;
			}

			const auto& stageJson = json[stageName];
			if (!stageJson.hasElement(U"backgrounds")) {
				return true; // 空でも成功とする
			}

			const auto& backgroundsArray = stageJson[U"backgrounds"];
			if (!backgroundsArray.isArray()) {
				return false;
			}

			for (const auto& bgJson : backgroundsArray.arrayView()) {
				Jam::Domain::Background::BackgroundObject bgObj;

				if (!parseBackgroundObject(bgJson, bgObj)) {
					continue;
				}

				outObjects.push_back(bgObj);
			}

			return true;
		}

	private:
		// 単一の背景オブジェクトをJSONからパース
		static bool parseBackgroundObject(const JSON& json, Jam::Domain::Background::BackgroundObject& outObj) {
			// 必須フィールドのチェック
			if (!json.hasElement(U"rect")) {
				return false;
			}
			if (!json.hasElement(U"textureName")) {
				return false;
			}

			// 位置・サイズの読み込み
			const auto& rectJson = json[U"rect"];

			if (!rectJson.isArray()) {
				return false;
			}

			if (rectJson.size() < 4) {
				return false;
			}

			try {
				outObj.rect = RectF(
					rectJson[0].get<double>(),
					rectJson[1].get<double>(),
					rectJson[2].get<double>(),
					rectJson[3].get<double>()
				);
			}
			catch (const std::exception& e) {
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



			// 透明度（オプション）
			if (json.hasElement(U"opacity")) {
				outObj.opacity = json[U"opacity"].get<double>();
			}
			
			// 拡張距離（オプション）
			if (json.hasElement(U"leftExtension")) {
				outObj.leftExtension = json[U"leftExtension"].get<double>();
			}
			if (json.hasElement(U"rightExtension")) {
				outObj.rightExtension = json[U"rightExtension"].get<double>();
			}
            
			return true;
		}
	};
}
