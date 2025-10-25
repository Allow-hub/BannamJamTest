#pragma once
#include <Siv3D.hpp>
#include "../Domain/Stage/StageTypes.h"

namespace Jam::Infrastructure::Stage {
	using StageObject = Jam::Domain::Stage::StageObject;
	using CollisionType = Jam::Domain::Stage::StageType;
	/**
	 * ステージデータJSONローダー
	 * ファイル読み込みとオブジェクト解析を担当
	 */
	class StageLoader {
	private:
		// 定数定義
		static constexpr size_t RECT_ARRAY_SIZE = 4;
		static constexpr size_t HEX_COLOR_LENGTH = 7;

	public:
		// ステージファイル読み込み
		static bool loadStageFromFile(const String& stageFileName, Array<StageObject>& outObjects) {
			const String stagePath = U"../Assets/Stage/" + stageFileName;
			return loadFromJson(stagePath, outObjects);
		}

		// ステージファイル読み込み（オブジェクトを種類別に分離して出力）
		static bool loadStageFromFile(const String& stageFileName, Array<StageObject>& outNormalObjects, Array<StageObject>& outMovingObjects) {
			Array<StageObject> allObjects;
			if (!loadStageFromFile(stageFileName, allObjects)) {
				return false;
			}

			// オブジェクトを種類別に分離
			outNormalObjects.clear();
			outMovingObjects.clear();

			for (const auto& obj : allObjects) {
				if (obj.type == Jam::Domain::Stage::StageType::MovingPlatform) {
					outMovingObjects.push_back(obj);
				}
				else {
					outNormalObjects.push_back(obj);
				}
			}

			return true;
		}

		// JSON読み込み
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
				}
				else {
					Print << U"オブジェクト " + ToString(totalCount) + U" の解析に失敗しました";
				}
			}

			// Print << U"ステージオブジェクトの読み込み完了: " + ToString(successCount) + U"/" + ToString(totalCount);
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
				: Jam::Domain::Stage::StageType::None;

			// メタデータ解析
			obj.metadata = objJson.hasElement(U"metadata")
				? objJson[U"metadata"].getString()
				: U"";

			// 地上判定の面を解析(デフォルトは上面のみ)
			obj.groundSide = objJson.hasElement(U"groundSide")
				? Jam::Domain::Stage::stringToGroundSide(objJson[U"groundSide"].getString())
				: Jam::Domain::Stage::GroundSide::Up;

			// 動くプラットフォーム用のデータ解析
			if (obj.type == Jam::Domain::Stage::StageType::MovingPlatform) {
				// 移動タイプの解析
				if (objJson.hasElement(U"movementType")) {
					obj.movementType = Jam::Domain::Stage::stringToMovementType(objJson[U"movementType"].getString());
				}
				else {
					obj.movementType = Jam::Domain::Stage::MovementType::Horizontal;
				}

				// 移動速度の解析
				if (objJson.hasElement(U"movementSpeed")) {
					obj.movementSpeed = objJson[U"movementSpeed"].get<double>();
				}

				// 移動距離の解析
				if (objJson.hasElement(U"movementDistance")) {
					obj.movementDistance = objJson[U"movementDistance"].get<double>();
				}

				// ループ設定の解析
				if (objJson.hasElement(U"loopMovement")) {
					obj.loopMovement = objJson[U"loopMovement"].get<bool>();
				}
			}

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
			}
			catch (...) {
				return false;
			}
		}
	};
}
