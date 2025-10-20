#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain
{
	// 立ち絵の種類
	enum class Portrait {
		Normal,
		Surprised,  //驚き
		Suspicious, //怪しむ
		Smiling,	//笑顔
		Conversation//会話
	};

	// 立つ場所の種類
	enum class Location {
		Left,
		Center,
		Right
	};

	// ストーリーデータの構造体
	struct StoryLine {
		int32 lineNumber;      // 流す行番号
		String speaker;        // 発話者
		Portrait portrait;     // 立ち絵のEnum
		String text;           // テキスト
		Location location;     // 場所のEnum
	};

	// シーン情報（同じ行番号のデータをまとめる）
	struct StoryScene {
		int32 lineNumber;
		Location location;
		Array<StoryLine> lines;  // 同じ行番号の複数のキャラクター
		String displayText;      // 表示するテキスト（最後の行を優先）
	};

	// Enumと文字列の変換テーブル
	namespace EnumConverter {
		const HashTable<String, Portrait> portraitTable = {
			{U"normal", Portrait::Normal},
			{U"surprised", Portrait::Surprised},
			{U"suspicious", Portrait::Suspicious},
			{U"smiling", Portrait::Smiling},
			{U"conversation", Portrait::Conversation}
		};

		const HashTable<String, Location> locationTable = {
			{U"left", Location::Left},
			{U"normal", Location::Center},
			{U"right", Location::Right}
		};

		Portrait toPortrait(const String& str) {
			String lower = str.lowercased();
			return portraitTable.contains(lower) ? portraitTable.at(lower) : Portrait::Normal;
		}

		Location toLocation(const String& str) {
			String lower = str.lowercased();
			return locationTable.contains(lower) ? locationTable.at(lower) : Location::Center;
		}
	}
}
