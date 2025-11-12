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

		String currentVisibleText;
		double textTimer = 0.0;
		double textSpeed = 0.03;
		bool isFullyVisible = false;
		bool isSkipping = false;

		ColorF activeSpeakerColor = ColorF(1.0);
		ColorF inactiveSpeakerColor = ColorF(0.5);

		Vec2 getPositionForLocation(Location location, const Vec2& portraitSize) const;
		Size getPortraitSize() const;
		RectF getTextBox() const;
		Speaker getCurrentSpeaker() const;
		bool loadFromCSV(const FilePath& path);
		void resetTextState();

	public:
		StoryManager();

		bool init(const FilePath& csvPath,
				  const HashTable<Speaker, HashTable<Portrait, FilePath>>& characterPortraits,
				  const FilePath& backgroundPath = U"");

		void setDimmingColors(const ColorF& activeColor, const ColorF& inactiveColor);

		void update(double deltaTime);
		void draw() const;

		String wrapText(const String& text, size_t maxCharsPerLine) const;

		void skip(bool enable);
		bool next();
		bool previous();
		bool isEnd() const;
		void reset();
		size_t getSceneCount() const;
		size_t getCurrentSceneIndex() const;
	};
}
