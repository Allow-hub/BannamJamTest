#pragma once
#include <Siv3D.hpp>
#include "Domain/Stage/StageTypes.h"

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
		static bool loadStageFromFile(const String& stageFileName, Array<StageObject>& outObjects);

		// ステージファイル読み込み(オブジェクトを種類別に分離して出力)
		static bool loadStageFromFile(const String& stageFileName, Array<StageObject>& outNormalObjects, Array<StageObject>& outMovingObjects);

		// JSON読み込み
		static bool loadFromJson(const String& jsonPath, Array<StageObject>& outObjects);

	private:
		// ステージオブジェクト解析
		static Optional<StageObject> parseStageObject(const JSON& objJson);

		// 矩形配列解析（配列形式[x,y,width,height]）
		static bool parseRect(const JSON& objJson, RectF& rect);
	};
}
