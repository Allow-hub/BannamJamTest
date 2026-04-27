#pragma once
#include <Siv3D.hpp>
#include "Domain/Block/BlockTypes.h"

namespace Jam::Infrastructure {

	class TextureLoader {
	public:
		static Texture getStageTexture(Jam::Domain::Block::BlockType type);
		static void preloadStageTextures();
		static void clearTextureCache();
		static bool loadCustomTexture(Jam::Domain::Block::BlockType type, const FilePath& resourcePath);

		// 背景テクスチャ関連（名前ベース）
		static Optional<Texture> getTexture(const String& textureName);
		static void preloadBackgroundTextures();
		static bool loadBackgroundTexture(const String& name, const FilePath& resourcePath);

	private:
		static HashTable<Jam::Domain::Block::BlockType, Texture> s_stageTextures;
		static HashTable<String, Texture> s_backgroundTextures;
		static bool s_initialized;

		static FilePath getDefaultTexturePath(Jam::Domain::Block::BlockType type);
		static FilePath getDefaultBackgroundTexturePath(const String& textureName);
		static void initialize();
	};
}
