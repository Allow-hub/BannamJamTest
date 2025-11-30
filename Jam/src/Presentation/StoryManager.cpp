#include "Presentation/StoryManager.h"

namespace Jam::Presentation
{
	using namespace Jam::Domain;

	StoryManager::StoryManager()
	{
		const int32 mainFontSize = static_cast<int32>(Scene::Height() * 0.05);
		font = Font(mainFontSize, U"Assets/Font/Corporate-Logo-Bold-ver3.otf", FontStyle::Bold);
	}

	Vec2 StoryManager::getPositionForLocation(Location location, const Vec2& portraitSize) const
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

	Size StoryManager::getPortraitSize() const
	{
		const double w = Scene::Width();
		const double h = Scene::Height();
		return Size(static_cast<int32>(w / 2.2), static_cast<int32>(h / 1.0));
	}

	RectF StoryManager::getTextBox() const
	{
		const double w = Scene::Width();
		const double h = Scene::Height();
		const double boxWidth = w * 0.95;
		const double boxHeight = h * 0.4;
		const double x = (w - boxWidth) / 2.0;
		const double y = h - boxHeight - 25.0;
		return RectF(x, y, boxWidth, boxHeight);
	}

	Speaker StoryManager::getCurrentSpeaker() const
	{
		if (scenes.isEmpty() || currentSceneIndex >= scenes.size())
			return Speaker::Player;

		const auto& scene = scenes[currentSceneIndex];
		if (scene.lines.isEmpty())
			return Speaker::Player;

		return EnumConverter::toSpeaker(scene.lines.back().speaker);
	}

	bool StoryManager::init(const FilePath& csvPath,
							const HashTable<Speaker, HashTable<Portrait, FilePath>>& characterPortraits,
							const FilePath& backgroundPath)
	{
		for (const auto& [speaker, portraits] : characterPortraits)
		{
			for (const auto& [portrait, path] : portraits)
			{
				portraitTextures[speaker][portrait] = Texture(Resource(path));
			}
		}

		if (!backgroundPath.isEmpty())
			backgroundTexture = Texture(Resource(backgroundPath));

		const bool ok = loadFromCSV(csvPath);
		if (ok) resetTextState();
		return ok;
	}

	void StoryManager::setDimmingColors(const ColorF& activeColor, const ColorF& inactiveColor)
	{
		activeSpeakerColor = activeColor;
		inactiveSpeakerColor = inactiveColor;
	}

	bool StoryManager::loadFromCSV(const FilePath& path)
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

	void StoryManager::resetTextState()
	{
		textTimer = 0.0;
		currentVisibleText.clear();
		isFullyVisible = false;
		isSkipping = false;
	}

	void StoryManager::update(double deltaTime)
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

	void StoryManager::draw() const
	{
		if (scenes.isEmpty() || currentSceneIndex >= scenes.size()) return;
		const auto& scene = scenes[currentSceneIndex];

		if (backgroundTexture) backgroundTexture.resized(Scene::Size().x, Scene::Size().y).draw();
		else Scene::Rect().draw(ColorF(0.8, 0.9, 1.0));

		const Speaker currentSpeaker = getCurrentSpeaker();

		for (const auto& line : scene.lines)
		{
			Speaker sp = EnumConverter::toSpeaker(line.speaker);
			if (portraitTextures.contains(sp))
			{
				const auto& portraits = portraitTextures.at(sp);
				if (portraits.contains(line.portrait))
				{
					const ColorF color = (sp == currentSpeaker) ? activeSpeakerColor : inactiveSpeakerColor;
					const Texture& tex = portraits.at(line.portrait);
					const double scale = (Scene::Height() * 1.0) / tex.height();
					const Vec2 position = getPositionForLocation(line.location, tex.size() * scale);

					tex.scaled(scale).drawAt(position, color);
				}
			}
		}

		const RectF box = getTextBox();
		box.draw(ColorF(0.0, 0.0, 0.0, 0.8));
		box.drawFrame(2, ColorF(1.0));

		if (!scene.lines.isEmpty())
		{
			Speaker sp = EnumConverter::toSpeaker(scene.lines.back().speaker);
			String name = EnumConverter::toString(sp);
			font(name).draw(box.x + 20, box.y + 20, ColorF(1.0, 0.8, 0.2));
		}

		font(wrapText(currentVisibleText, 35)).draw(box.x + 20, box.y + 110, ColorF(1.0));

		font(U"Progress: {}/{}"_fmt(currentSceneIndex + 1, scenes.size()))
			.draw(Arg::bottomRight = Vec2(Scene::Width() - 20, Scene::Height() - 20), ColorF(1.0));
	}

	String StoryManager::wrapText(const String& text, size_t maxCharsPerLine) const
	{
		String wrapped;
		size_t count = 0;
		for (auto ch : text)
		{
			wrapped << ch;
			++count;
			if (count >= maxCharsPerLine && ch != U'\n')
			{
				wrapped << U'\n';
				count = 0;
			}
		}
		return wrapped;
	}

	void StoryManager::skip(bool enable) { isSkipping = enable; }

	bool StoryManager::next()
	{
		if (scenes.isEmpty()) return false;

		if (!isFullyVisible)
		{
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

	bool StoryManager::previous()
	{
		if (currentSceneIndex > 0)
		{
			--currentSceneIndex;
			resetTextState();
			return true;
		}
		return false;
	}

	bool StoryManager::isEnd() const { return currentSceneIndex >= scenes.size() - 1; }
	void StoryManager::reset() { currentSceneIndex = 0; resetTextState(); }
	size_t StoryManager::getSceneCount() const { return scenes.size(); }
	size_t StoryManager::getCurrentSceneIndex() const { return currentSceneIndex; }
}
