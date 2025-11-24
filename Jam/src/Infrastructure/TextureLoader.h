#pragma once
#include <Siv3D.hpp>
#include "../Domain/Stage/StageTypes.h"

/**
 * ステージテクスチャ読み込み・管理クラス
 * StageTypeに対応したテクスチャの読み込みとキャッシュを行う（完全Resource対応版）
 */
namespace Jam::Infrastructure {

	class TextureLoader {
	public:
		static Texture getStageTexture(Jam::Domain::Stage::StageType type);
		static void preloadStageTextures();
		static void clearTextureCache();

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

	// === 静的メンバの定義 ===
	HashTable<Jam::Domain::Stage::StageType, Texture> TextureLoader::s_stageTextures;
	HashTable<String, Texture> TextureLoader::s_backgroundTextures;
	bool TextureLoader::s_initialized = false;

	// ステージテクスチャを取得
	Texture TextureLoader::getStageTexture(Jam::Domain::Stage::StageType type) {
		if (!s_initialized) {
			initialize();
		}

		// キャッシュ済みなら即返す
		if (s_stageTextures.contains(type)) {
			return s_stageTextures[type];
		}

		const FilePath resourcePath = getDefaultTexturePath(type);
		Texture texture(Resource(resourcePath));

		if (texture) {
			s_stageTextures[type] = texture;
			return texture;
		}

		// Resourceに失敗した場合（登録漏れなど）
		Console << U"[TextureLoader] Failed to load Resource: " << resourcePath;
		return Texture{};
	}

	// すべてのステージテクスチャを事前読み込み
	void TextureLoader::preloadStageTextures() {
		const Array<Jam::Domain::Stage::StageType> stageTypes = {
			Jam::Domain::Stage::StageType::Normal,
			Jam::Domain::Stage::StageType::MovingPlatform,
			Jam::Domain::Stage::StageType::OneWayPlatform,
			Jam::Domain::Stage::StageType::DamagePlatform,
			Jam::Domain::Stage::StageType::MovingDamagePlatform,
		};

		for (auto type : stageTypes) {
			getStageTexture(type);
		}
	}

	// キャッシュクリア
	void TextureLoader::clearTextureCache() {
		s_stageTextures.clear();
		s_backgroundTextures.clear();
		s_initialized = false;
	}

	// デフォルトのステージパス
	FilePath TextureLoader::getDefaultTexturePath(Jam::Domain::Stage::StageType type) {
		const FilePath basePath = U"Assets/Stage/";

		switch (type) {
		case Jam::Domain::Stage::StageType::Normal:
			return basePath + U"normal_stage.png";
		case Jam::Domain::Stage::StageType::MovingPlatform:
			return basePath + U"moving_platform.png";
		case Jam::Domain::Stage::StageType::OneWayPlatform:
			return basePath + U"normal_stage.png";
		case Jam::Domain::Stage::StageType::DamagePlatform:
			return basePath + U"damage_Stage.jpg";
		case Jam::Domain::Stage::StageType::MovingDamagePlatform:
			return basePath + U"damage_Stage.jpg";
		default:
			return basePath + U"normal_stage.png";
		}
	}

	// 背景テクスチャ取得
	Optional<Texture> TextureLoader::getTexture(const String& textureName) {
		if (!s_initialized) {
			initialize();
		}

		if (s_backgroundTextures.contains(textureName)) {
			return s_backgroundTextures[textureName];
		}

		const FilePath resourcePath = getDefaultBackgroundTexturePath(textureName);
		Texture texture(Resource(resourcePath));

		if (texture) {
			s_backgroundTextures[textureName] = texture;
			return texture;
		}
		return none;
	}

	void TextureLoader::preloadBackgroundTextures() {
		const Array<String> backgroundNames = { U"BG", U"BG1", U"BG2", U"BG3" };
		for (const auto& name : backgroundNames) {
			getTexture(name);
		}
	}

	bool TextureLoader::loadBackgroundTexture(const String& name, const FilePath& resourcePath) {
		Texture texture(Resource(resourcePath));
		if (!texture) {
			return false;
		}
		s_backgroundTextures[name] = texture;
		return true;
	}

	FilePath TextureLoader::getDefaultBackgroundTexturePath(const String& textureName) {
		const FilePath basePath = U"Assets/Stage/BG/";

		if (textureName == U"BG") {
			return basePath + U"BG.png";
		}
		if (textureName == U"BG1" || textureName == U"BG_back1") {
			return basePath + U"BG_Back1.png";
		}
		if (textureName == U"BG2" || textureName == U"BG_back2") {
			return basePath + U"BG_Back2.png";
		}
		if (textureName == U"BG3" || textureName == U"BG_back3") {
			return basePath + U"BG_Back3.png";
		}
		return basePath + textureName + U".png";
	}

	void TextureLoader::initialize() {
		if (s_initialized) return;
		s_initialized = true;
	}
}
