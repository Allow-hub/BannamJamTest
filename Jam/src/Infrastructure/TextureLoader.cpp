#include "Infrastructure/TextureLoader.h"

namespace Jam::Infrastructure {

	// 静的メンバ定義
	HashTable<Jam::Domain::Stage::StageType, Texture> TextureLoader::s_stageTextures;
	HashTable<String, Texture> TextureLoader::s_backgroundTextures;
	bool TextureLoader::s_initialized = false;

	Texture TextureLoader::getStageTexture(Jam::Domain::Stage::StageType type) {
		if (!s_initialized) initialize();

		if (s_stageTextures.contains(type)) {
			return s_stageTextures[type];
		}

		const FilePath resourcePath = getDefaultTexturePath(type);
		Texture texture(Resource(resourcePath));

		if (texture) {
			s_stageTextures[type] = texture;
			return texture;
		}

		assert(false && "Failed to load stage texture. Check if the texture file exists.");
		return Texture{};
	}

	void TextureLoader::preloadStageTextures() {
		const Array<Jam::Domain::Stage::StageType> stageTypes = {
			Jam::Domain::Stage::StageType::Normal,
			Jam::Domain::Stage::StageType::MovingPlatform,
			Jam::Domain::Stage::StageType::OneWayPlatform,
			Jam::Domain::Stage::StageType::DamagePlatform,
			Jam::Domain::Stage::StageType::MovingDamagePlatform,
		};

		for (auto type : stageTypes) getStageTexture(type);
	}

	void TextureLoader::clearTextureCache() {
		s_stageTextures.clear();
		s_backgroundTextures.clear();
		s_initialized = false;
	}

	bool TextureLoader::loadCustomTexture(Jam::Domain::Stage::StageType type, const FilePath& resourcePath) {
		Texture texture(Resource(resourcePath));
		
		if (texture) {
			s_stageTextures[type] = texture;
			return true;
		}

		assert(false && "Failed to load custom texture. Check if the texture file exists.");
		return false;
	}

	FilePath TextureLoader::getDefaultTexturePath(Jam::Domain::Stage::StageType type) {
		const FilePath basePath = U"Assets/Stage/";
		switch (type) {
		case Jam::Domain::Stage::StageType::Normal: return basePath + U"NormalStage_1.png";
		case Jam::Domain::Stage::StageType::MovingPlatform: return basePath + U"MovingPlatform.png";
		case Jam::Domain::Stage::StageType::OneWayPlatform: return basePath + U"NormalStage_1.png";
		case Jam::Domain::Stage::StageType::DamagePlatform: return basePath + U"DamageStage.jpg";
		case Jam::Domain::Stage::StageType::MovingDamagePlatform: return basePath + U"DamageStage.jpg";
		default: return basePath + U"NormalStage_1.png";
		}
	}

	Optional<Texture> TextureLoader::getTexture(const String& textureName) {
		if (!s_initialized) initialize();

		if (s_backgroundTextures.contains(textureName)) return s_backgroundTextures[textureName];

		const FilePath resourcePath = getDefaultBackgroundTexturePath(textureName);
		Texture texture(Resource(resourcePath));
		
		if (texture) {
			s_backgroundTextures[textureName] = texture;
			return texture;
		}

		assert(false && "Failed to load background texture. Check if the texture file exists.");
		return none;
	}

	void TextureLoader::preloadBackgroundTextures() {
		const Array<String> backgroundNames = { U"BG", U"BG1", U"BG2", U"BG3" };
		for (const auto& name : backgroundNames) getTexture(name);
	}

	bool TextureLoader::loadBackgroundTexture(const String& name, const FilePath& resourcePath) {
		Texture texture(Resource(resourcePath));
		
		if (texture) {
			s_backgroundTextures[name] = texture;
			return true;
		}

		assert(false && "Failed to load background texture. Check if the texture file exists.");
		return false;
	}

	FilePath TextureLoader::getDefaultBackgroundTexturePath(const String& textureName) {
		const FilePath basePath = U"Assets/Stage/BG/";
		// すべて現在存在するBackground.pngを使用
		return basePath + U"Background.png";
	}

	void TextureLoader::initialize() {
		if (s_initialized) return;
		s_initialized = true;
	}

}
