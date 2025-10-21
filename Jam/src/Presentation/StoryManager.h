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

		HashTable<Speaker, HashTable<Portrait, Texture>> portraitTextures;
		Texture backgroundTexture;

		// === 新規追加 ===
		String currentVisibleText;   // 現在表示中の文字
		double textTimer = 0.0;      // 経過時間
		double textSpeed = 0.03;     // 1文字あたりの間隔（秒）
		bool isFullyVisible = false; // 現在のセリフが全文表示済みか
		bool isSkipping = false;     // スキップ中か

		// === 位置・サイズ関連 ===
		Vec2 getPositionForLocation(Location location, const Size& portraitSize) const
		{
			const double w = Scene::Width();
			const double h = Scene::Height();
			const double y = h * 0.57;

			switch (location)
			{
			case Location::Left:   return Vec2(w * 0.25, y);
			case Location::Center: return Vec2(w * 0.5, y);
			case Location::Right:  return Vec2(w * 0.75, y);
			default:               return Vec2(w * 0.5, y);
			}
		}

		Size getPortraitSize() const
		{
			const double w = Scene::Width();
			const double h = Scene::Height();
			return Size(static_cast<int32>(w / 2.2), static_cast<int32>(h / 1.0));
		}

		RectF getTextBox() const
		{
			const double w = Scene::Width();
			const double h = Scene::Height();
			const double boxWidth = w * 0.95;
			const double boxHeight = h * 0.4;
			const double x = (w - boxWidth) / 2.0;
			const double y = h - boxHeight - 25.0;
			return RectF(x, y, boxWidth, boxHeight);
		}

	public:
		StoryManager()
		{
			const int32 mainFontSize = static_cast<int32>(Scene::Height() * 0.05);
			font = Font(mainFontSize, U"../Assets/Font/PixelMplus12-Bold.ttf", FontStyle::Italic);
		}

		bool init(const FilePath& csvPath,
				  const HashTable<Speaker, HashTable<Portrait, FilePath>>& characterPortraits,
				  const FilePath& backgroundPath = U"")
		{
			// === 立ち絵テクスチャ ===
			for (const auto& [speaker, portraits] : characterPortraits)
			{
				for (const auto& [portrait, path] : portraits)
				{
					portraitTextures[speaker][portrait] = Texture(path);
				}
			}

			// === 背景 ===
			if (!backgroundPath.isEmpty())
			{
				backgroundTexture = Texture(backgroundPath);
			}

			// === ストーリー読み込み ===
			const bool ok = loadFromCSV(csvPath);
			if (ok) resetTextState();
			return ok;
		}

	private:
		bool loadFromCSV(const FilePath& path)
		{
			TextReader reader(path);
			if (!reader) return false;

			String header;
			reader.readLine(header);

			Array<StoryLine> rawLines;
			String line;

			while (reader.readLine(line))
			{
				if (line.isEmpty()) continue;
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

			if (rawLines.isEmpty()) return false;

			int32 currentLineNumber = -1;
			StoryScene currentScene;

			for (const auto& storyLine : rawLines)
			{
				if (storyLine.lineNumber != currentLineNumber)
				{
					if (currentLineNumber != -1)
						scenes << currentScene;

					currentScene = StoryScene{};
					currentScene.lineNumber = storyLine.lineNumber;
					currentScene.location = storyLine.location;
					currentScene.lines.clear();
					currentLineNumber = storyLine.lineNumber;
				}

				currentScene.lines << storyLine;
				currentScene.displayText = storyLine.text;
			}
			if (!currentScene.lines.isEmpty()) scenes << currentScene;

			return true;
		}

		// === テキスト状態をリセット ===
		void resetTextState()
		{
			textTimer = 0.0;
			currentVisibleText.clear();
			isFullyVisible = false;
			isSkipping = false;
		}

	public:
		// === 毎フレーム更新 ===
		void update(double deltaTime)
		{
			if (scenes.isEmpty() || currentSceneIndex >= scenes.size()) return;
			const auto& scene = scenes[currentSceneIndex];

			if (isSkipping)
			{
				currentVisibleText = scene.displayText;
				isFullyVisible = true;
				return;
			}

			if (isFullyVisible) return;

			textTimer += deltaTime;

			const size_t visibleCount = static_cast<size_t>(textTimer / textSpeed);
			if (visibleCount >= scene.displayText.size())
			{
				currentVisibleText = scene.displayText;
				isFullyVisible = true;
			}
			else
			{
				currentVisibleText = scene.displayText.substr(0, visibleCount);
			}
		}

		// === 描画 ===
		void draw() const
		{
			if (scenes.isEmpty() || currentSceneIndex >= scenes.size()) return;
			const auto& scene = scenes[currentSceneIndex];

			// 背景
			if (backgroundTexture) backgroundTexture.resized(Scene::Size()).draw();
			else Scene::Rect().draw(ColorF(0.8, 0.9, 1.0));

			const Size portraitSize = getPortraitSize();

			// 立ち絵
			for (const auto& line : scene.lines)
			{
				Speaker sp = EnumConverter::toSpeaker(line.speaker);
				if (portraitTextures.contains(sp))
				{
					const auto& portraits = portraitTextures.at(sp);
					if (portraits.contains(line.portrait))
					{
						portraits.at(line.portrait)
							.resized(portraitSize)
							.drawAt(getPositionForLocation(line.location, portraitSize));
					}
				}
			}

			// テキストボックス
			const RectF box = getTextBox();
			box.draw(ColorF(0.0, 0.0, 0.0, 0.8));
			box.drawFrame(2, ColorF(1.0));

			// 発話者名
			if (!scene.lines.isEmpty())
			{
				Speaker sp = EnumConverter::toSpeaker(scene.lines.back().speaker);
				String name = EnumConverter::toString(sp);
				font(name).draw(box.x + 20, box.y + 20, ColorF(1.0, 0.8, 0.2));
			}

			// 1文字ずつ送りテキスト
			font(currentVisibleText).draw(box.x + 20, box.y + 90, ColorF(1.0));

			// 進行表示
			font(U"Progress: {}/{}"_fmt(currentSceneIndex + 1, scenes.size()))
				.draw(Arg::bottomRight = Vec2(Scene::Width() - 20, Scene::Height() - 20), ColorF(1.0));
		}

		// === スキップ制御 ===
		void skip(bool enable)
		{
			isSkipping = enable;
		}

		// === 次のシーンへ ===
		bool next()
		{
			if (scenes.isEmpty()) return false;

			if (!isFullyVisible)
			{
				// 全文未表示 → 即全文表示
				isSkipping = true;
				return true;
			}

			if (currentSceneIndex < scenes.size() - 1)
			{
				++currentSceneIndex;
				resetTextState();
				return true;
			}
			return false;
		}

		bool previous()
		{
			if (currentSceneIndex > 0)
			{
				--currentSceneIndex;
				resetTextState();
				return true;
			}
			return false;
		}

		bool isEnd() const { return currentSceneIndex >= scenes.size() - 1; }
		void reset() { currentSceneIndex = 0; resetTextState(); }
		size_t getSceneCount() const { return scenes.size(); }
		size_t getCurrentSceneIndex() const { return currentSceneIndex; }
	};
}
