#pragma once
#include <Siv3D.hpp>
#include "../Domain/StoryData.h"

namespace Jam::Presentation
{
	using namespace Jam::Domain;

	class StoryManager
	{
	private:
		Array<StoryScene> scenes;
		size_t currentSceneIndex = 0;
		Font font;
		// 立ち絵のテクスチャマップ（キャラクター名 -> 表情 -> テクスチャ）
		HashTable<String, HashTable<Portrait, Texture>> portraitTextures;

		// 背景テクスチャ
		Texture backgroundTexture;

		// Locationに応じた立ち絵の座標を取得
		Vec2 getPositionForLocation(Location location, const Size& portraitSize) const {
			const double w = Scene::Width();
			const double h = Scene::Height();

			const double y = h * 0.57; // 画面中央付近に表示
			switch (location) {
			case Location::Left:
				return Vec2(w * 0.25, y);
			case Location::Center:
				return Vec2(w * 0.5, y);
			case Location::Right:
				return Vec2(w * 0.75, y);
			default:
				return Vec2(w * 0.5, y);
			}
		}

		// 立ち絵のサイズを取得
		Size getPortraitSize() const {
			const double w = Scene::Width();
			const double h = Scene::Height();

			// 画面幅の1/3、画面高さくらい
			return Size(static_cast<int32>(w / 2.3), static_cast<int32>(h / 1.0));
		}

		// テキストボックスの矩形（画面下に横幅より少し小さいサイズで）
		RectF getTextBox() const {
			const double w = Scene::Width();
			const double h = Scene::Height();

			const double boxWidth = w * 0.95;   // 横幅は画面の95%
			const double boxHeight = h * 0.4;  // 高さは画面の40%
			const double x = (w - boxWidth) / 2.0;
			const double y = h - boxHeight - 25.0; // 下から少し余白

			return RectF(x, y, boxWidth, boxHeight);
		}
	public:
		StoryManager()
		{
			// 画面幅に応じてフォントサイズを決定
			const int32 mainFontSize = static_cast<int32>(Scene::Height() * 0.05); // 高さの4%くらい
			const int32 nameFontSize = static_cast<int32>(Scene::Height() * 0.03);

			// TTFをロード
			font = Font(mainFontSize, U"../Assets/Font/PixelMplus12-Bold.ttf", FontStyle::Italic);
		}

		// 初期化（テクスチャの読み込みとCSVの読み込み）
		bool init(const FilePath& csvPath,
				  const HashTable<String, HashTable<Portrait, FilePath>>& characterPortraits,
				  const FilePath& backgroundPath = U"") {

			// 立ち絵テクスチャを読み込む
			for (const auto& [characterName, portraits] : characterPortraits) {
				for (const auto& [portrait, path] : portraits) {
					portraitTextures[characterName][portrait] = Texture(path);
					Console << U"Loaded portrait texture [" << characterName << U"][" << static_cast<int>(portrait) << U"]: " << path;
				}
			}

			// 背景テクスチャを読み込む
			if (!backgroundPath.isEmpty()) {
				backgroundTexture = Texture(backgroundPath);
				Console << U"Loaded background texture: " << backgroundPath;
			}

			// CSVからストーリーデータを読み込む
			return loadFromCSV(csvPath);
		}

		// CSVファイルからストーリーデータを読み込む（内部用）
	private:
		bool loadFromCSV(const FilePath& path) {
			TextReader reader(path);

			if (!reader) {
				Console << U"Failed to open CSV file: " << path;
				return false;
			}

			// 1行目（ヘッダー）をスキップ
			String header;
			if (!reader.readLine(header)) {
				Console << U"CSV file is empty";
				return false;
			}

			Array<StoryLine> rawLines;
			String line;

			// 各行を読み込む
			while (reader.readLine(line)) {
				if (line.isEmpty()) continue;

				// カンマで分割
				Array<String> columns = line.split(U',');

				if (columns.size() < 5) continue;

				StoryLine storyLine;
				storyLine.lineNumber = ParseOr<int32>(columns[0].trimmed(), 0);
				storyLine.speaker = columns[1].trimmed();
				storyLine.portrait = EnumConverter::toPortrait(columns[2].trimmed());
				storyLine.text = columns[3].trimmed();
				storyLine.location = EnumConverter::toLocation(columns[4].trimmed());

				rawLines << storyLine;
			}

			// 同じ行番号のデータをまとめてシーンを作成
			if (rawLines.isEmpty()) {
				Console << U"No valid story data found";
				return false;
			}

			int32 currentLineNumber = -1;
			StoryScene currentScene;

			for (const auto& storyLine : rawLines) {
				if (storyLine.lineNumber != currentLineNumber) {
					// 新しいシーンの開始
					if (currentLineNumber != -1) {
						scenes << currentScene;
					}

					currentScene = StoryScene{};
					currentScene.lineNumber = storyLine.lineNumber;
					currentScene.location = storyLine.location;
					currentScene.lines.clear();
					currentLineNumber = storyLine.lineNumber;
				}

				// 同じ行番号のラインを追加
				currentScene.lines << storyLine;
				// テキストは最後の行を優先
				currentScene.displayText = storyLine.text;
			}

			// 最後のシーンを追加
			if (!currentScene.lines.isEmpty()) {
				scenes << currentScene;
			}

			Console << U"Loaded " << scenes.size() << U" scenes";
			return true;
		}

	public:
		// 立ち絵テクスチャを個別に読み込む
		void loadPortrait(const String& characterName, Portrait portrait, const FilePath& path) {
			if (!portraitTextures.contains(characterName)) {
				portraitTextures[characterName] = {}; // キャラ名キーがなければ初期化
			}
			portraitTextures[characterName][portrait] = Texture(path);
			Print << U"Loaded portrait texture [" << characterName << U"][" << static_cast<int>(portrait) << U"]: " << path;
		}

		// 背景テクスチャを設定
		void draw() const {
			if (scenes.isEmpty() || currentSceneIndex >= scenes.size()) return;

			const auto& scene = scenes[currentSceneIndex];

			// 背景
			if (backgroundTexture) {
				backgroundTexture.resized(Scene::Size()).draw();
			}
			else {
				Scene::Rect().draw(ColorF(0.8, 0.9, 1.0));
			}

			// 立ち絵
			const Size portraitSize = getPortraitSize();
			for (const auto& line : scene.lines) {
				if (portraitTextures.contains(line.speaker)) {
					const auto& portraits = portraitTextures.at(line.speaker);
					if (portraits.contains(line.portrait)) {
						const Texture& texture = portraits.at(line.portrait);
						texture.resized(portraitSize)
							.drawAt(getPositionForLocation(line.location, portraitSize));
					}
				}
			}

			// テキストボックス
			const RectF box = getTextBox();
			box.draw(ColorF(0.0, 0.0, 0.0, 0.8));
			box.drawFrame(2, ColorF(1.0, 1.0, 1.0));

			// 発話者名
			if (!scene.lines.isEmpty()) {
				const String& speaker = scene.lines.back().speaker;
				font(speaker).draw(box.x + 20, box.y + 20, ColorF(1.0, 0.8, 0.2));
			}

			// テキスト
			font(scene.displayText).draw(box.x + 20, box.y + 90, ColorF(1.0));

			// 進行状況（右下に表示）
			font(U"Scene: {}/{}"_fmt(currentSceneIndex + 1, scenes.size()))
				.draw(Arg::bottomRight = Vec2(Scene::Width() - 20, Scene::Height() - 20), ColorF(1.0));
		}


		// 次のシーンへ
		bool next() {
			if (currentSceneIndex < scenes.size() - 1) {
				currentSceneIndex++;
				return true;
			}
			return false;
		}

		// 前のシーンへ
		bool previous() {
			if (currentSceneIndex > 0) {
				currentSceneIndex--;
				return true;
			}
			return false;
		}

		// ストーリーが終了したか
		bool isEnd() const {
			return currentSceneIndex >= scenes.size() - 1;
		}

		// リセット
		void reset() {
			currentSceneIndex = 0;
		}

		// シーンの総数を取得
		size_t getSceneCount() const {
			return scenes.size();
		}

		// 現在のシーン番号を取得
		size_t getCurrentSceneIndex() const {
			return currentSceneIndex;
		}
	};
}
