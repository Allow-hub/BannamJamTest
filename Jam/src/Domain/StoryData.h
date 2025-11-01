#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain
{
	enum class Speaker
	{
		Player,   // 地雷ちゃん
		Owner,    // マインドダイブされる側
		Riska     // リスカちゃん
	};

	enum class Portrait {
		Normal,
		Surprised,
		Suspicious,
		Smiling,
		Conversation
	};

	enum class Location {
		Left,
		Center,
		Right
	};

	struct StoryLine {
		int32 lineNumber;
		String speaker;
		Portrait portrait;
		String text;
		Location location;
	};

	struct StoryScene {
		int32 lineNumber;
		Location location;
		Array<StoryLine> lines;
		String displayText;
	};

	namespace EnumConverter
	{
		// --- Portrait ---
		const HashTable<String, Portrait> portraitTable = {
			{U"normal", Portrait::Normal},
			{U"surprised", Portrait::Surprised},
			{U"suspicious", Portrait::Suspicious},
			{U"smiling", Portrait::Smiling},
			{U"conversation", Portrait::Conversation}
		};

		// --- Location ---
		const HashTable<String, Location> locationTable = {
			{U"left", Location::Left},
			{U"center", Location::Center},
			{U"right", Location::Right}
		};

		// --- Speaker ---
		const HashTable<String, Speaker> speakerTable = {
			{U"player", Speaker::Player},
			{U"owner", Speaker::Owner},
			{U"riska", Speaker::Riska}
		};

		// --- String → Enum ---
		inline Portrait toPortrait(const String& str)
		{
			String lower = str.lowercased();
			return portraitTable.contains(lower) ? portraitTable.at(lower) : Portrait::Normal;
		}

		inline Location toLocation(const String& str)
		{
			String lower = str.lowercased();
			return locationTable.contains(lower) ? locationTable.at(lower) : Location::Center;
		}

		inline Speaker toSpeaker(const String& str)
		{
			String lower = str.lowercased();
			return speakerTable.contains(lower) ? speakerTable.at(lower) : Speaker::Owner;
		}

		inline String toString(Speaker speaker)
		{
			switch (speaker)
			{
			case Speaker::Player: return U"YUMEMIちゃん";
			case Speaker::Owner:  return U"小島 シンイチ";
			case Speaker::Riska:  return U"リスカちゃん";
			default:              return U"？？？";
			}
		}

		inline String toString(Portrait portrait)
		{
			for (const auto& [key, val] : portraitTable)
				if (val == portrait) return key;
			return U"normal";
		}

		inline String toString(Location location)
		{
			for (const auto& [key, val] : locationTable)
				if (val == location) return key;
			return U"center";
		}
	}
}
