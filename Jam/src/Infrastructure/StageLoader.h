#pragma once
#include <Siv3D.hpp>
#include "../Domain/Stage/StageTypes.h"

namespace Jam::Infrastructure::Stage {

    using StageObject = Jam::Domain::Stage::StageObject;
    using CollisionType = Jam::Domain::Stage::CollisionType;

	class StageLoader {
	public:
		// JSON読み込み（静的メソッドで単純化）
		static bool loadFromJson(const String& jsonPath, Array<StageObject>& outObjects) {
			// ファイル存在確認
			if (!FileSystem::Exists(jsonPath)) {
				return false;
			}

			JSON json = JSON::Load(jsonPath);
			if (!json) {
				return false;
			}

			outObjects.clear();

			if (!json.hasElement(U"objects")) {
				return false;
			}

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
