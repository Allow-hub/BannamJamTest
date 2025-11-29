#pragma once
#include <Siv3D.hpp>
#include "Domain/Stage/StageTypes.h"

namespace Jam::Infrastructure {

	class TextureLoader {
	public:
		static Texture getStageTexture(Jam::Domain::Stage::StageType type);
		static void preloadStageTextures();
		static void clearTextureCache();
		static bool loadCustomTexture(Jam::Domain::Stage::StageType type, const FilePath& resourcePath);

		// 背景テクスチャ関連（名前ベース）
		static Optional<Texture> getTexture(const String& textureName);
		static void preloadBackgroundTextures();
		static bool loadBackgroundTexture(const String& name, const FilePath& resourcePath);

	private:
		static HashTable<Jam::Domain::Stage::StageType, Texture> s_stageTextures;
		static HashTable<String, Texture> s_backgroundTextures;
		static bool s_initialized;

		static FilePath getDefaultTexturePath(Jam::Domain::Stage::StageType type);
		static FilePath getDefaultBackgroundTexturePath(const String& textureName);
		static void initialize();
	};
}
