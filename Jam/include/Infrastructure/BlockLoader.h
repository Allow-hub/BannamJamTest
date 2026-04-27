#pragma once
#include <Siv3D.hpp>
#include "Domain/Block/BlockTypes.h"

namespace Jam::Infrastructure::Block {
	using BlockObject = Jam::Domain::Block::BlockObject;
	using CollisionType = Jam::Domain::Block::BlockType;
	
	/**
	 * ステージデータJSONローダー
	 * ファイル読み込みとオブジェクト解析を担当
	 */
	class BlockLoader {
	private:
		// 定数定義
		static constexpr size_t RECT_ARRAY_SIZE = 4;
		static constexpr size_t HEX_COLOR_LENGTH = 7;

	public:
		// ステージファイル読み込み
		static bool loadBlockFromFile(const String& stageFileName, Array<BlockObject>& outObjects);

		// ステージファイル読み込み(オブジェクトを種類別に分離して出力)
		static bool loadBlockFromFile(const String& stageFileName, Array<BlockObject>& outNormalObjects, Array<BlockObject>& outMovingObjects);

		// JSON読み込み
		static bool loadFromJson(const String& jsonPath, Array<BlockObject>& outObjects);

	private:
		// ステージオブジェクト解析
		static Optional<BlockObject> parseBlockObject(const JSON& objJson);

		// 矩形配列解析（配列形式[x,y,width,height]）
		static bool parseRect(const JSON& objJson, RectF& rect);
	};
}
