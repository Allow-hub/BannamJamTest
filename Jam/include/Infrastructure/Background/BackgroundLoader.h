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
		static bool loadBackgroundFromFile(const String& fileName, Jam::Foundation::StageName stageName, Array<Jam::Domain::Background::BackgroundObject>& outObjects);
		
		// JSONファイルから背景データを読み込み（文字列版）
		static bool loadBackgroundFromFile(const String& fileName, const String& stageName, Array<Jam::Domain::Background::BackgroundObject>& outObjects);

	private:
		// 単一の背景オブジェクトをJSONからパース
		static bool parseBackgroundObject(const JSON& json, Jam::Domain::Background::BackgroundObject& outObj);
	};
}
