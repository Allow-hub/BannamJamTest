#pragma once
#include <Siv3D.hpp>
#include "../Domain/Stage/StageTypes.h"

namespace Jam::Infrastructure::Stage {

    using StageObject = Jam::Domain::Stage::StageObject;
    using CollisionType = Jam::Domain::Stage::CollisionType;

	class StageLoader {
	public:
		// ステージファイル読み込み（相対パス使用）
		static bool loadStageFromFile(const String& stageFileName, Array<StageObject>& outObjects) {
			String stagePath = U"Stage/" + stageFileName;  // App/は削除
			Print << U"StageLoader::loadStageFromFile: 試行パス = " << stagePath;
			bool result = loadFromJson(stagePath, outObjects);
			Print << U"StageLoader::loadStageFromFile: 結果 = " << (result ? U"成功" : U"失敗");
			return result;
		}
		
		// JSON読み込み（静的メソッドで単純化）
		static bool loadFromJson(const String& jsonPath, Array<StageObject>& outObjects) {
			Print << U"StageLoader::loadFromJson: パス = " << jsonPath;
			
			// ファイル存在確認
			if (!FileSystem::Exists(jsonPath)) {
				Print << U"StageLoader::loadFromJson: ファイルが存在しません";
				Print << U"StageLoader::loadFromJson: カレントディレクトリ = " << FileSystem::CurrentDirectory();
				return false;
			}
			
			Print << U"StageLoader::loadFromJson: ファイル存在確認OK";

			JSON json = JSON::Load(jsonPath);
			if (!json) {
				Print << U"StageLoader::loadFromJson: JSON解析に失敗";
				return false;
			}
			
			Print << U"StageLoader::loadFromJson: JSON解析成功";
			outObjects.clear();

			if (!json.hasElement(U"objects")) {
				Print << U"StageLoader::loadFromJson: 'objects'要素が見つかりません";
				return false;
			}
			
			Print << U"StageLoader::loadFromJson: 'objects'要素が見つかりました";

			for (const auto& objJson : json[U"objects"].arrayView()) {
				StageObject obj;

				// 配列形式rect読み込み [x, y, width, height]
				if (objJson.hasElement(U"rect") && objJson[U"rect"].isArray()) {
					const auto& rectArray = objJson[U"rect"].arrayView();
					Array<JSON> elements;
					for (const auto& elem : rectArray) {
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
				}

				// type読み込み
				if (objJson.hasElement(U"type")) {
					obj.type = stringToCollisionType(objJson[U"type"].getString());
				}

				// color読み込み
				if (objJson.hasElement(U"color")) {
					obj.color = parseColor(objJson[U"color"].getString());
				}
				else {
					obj.color = Palette::Gray;
				}

				// metadata読み込み
				if (objJson.hasElement(U"metadata")) {
					obj.metadata = objJson[U"metadata"].getString();
				}

				// destructible読み込み
				if (objJson.hasElement(U"destructible")) {
					obj.destructible = objJson[U"destructible"].get<bool>();
				}

				outObjects << obj;
			}

			Print << U"StageLoader::loadFromJson: " << outObjects.size() << U"個のオブジェクトを読み込み完了";
			return true;
		}

	private:
		static CollisionType stringToCollisionType(const String& typeStr) {
			if (typeStr == U"solid") return CollisionType::Solid;
			if (typeStr == U"platform") return CollisionType::Platform;
			if (typeStr == U"hazard") return CollisionType::Hazard;
			if (typeStr == U"trigger") return CollisionType::Trigger;
			if (typeStr == U"breakable") return CollisionType::Breakable;
			return CollisionType::None;
		}

		static Color parseColor(const String& colorStr) {
			if (colorStr.starts_with(U'#') && colorStr.length() == 7) {
				try {
					String hexStr = colorStr.substr(1);
					uint32 colorValue = ParseInt<uint32>(hexStr, 16);
					return Color(
						(colorValue >> 16) & 0xFF,
						(colorValue >> 8) & 0xFF,
						colorValue & 0xFF
					);
				}
				catch (...) {
					return Palette::Gray;
				}
			}
			return Palette::Gray;
		}
	};
}
