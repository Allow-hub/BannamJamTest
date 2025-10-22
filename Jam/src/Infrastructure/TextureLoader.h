#pragma once
#include <Siv3D.hpp>
#include "../Domain/Stage/StageTypes.h"

/**
 * ステージテクスチャ読み込み・管理クラス
 * StageTypeに対応したテクスチャの読み込みとキャッシュを行う
 */
namespace Jam::Infrastructure {
    
    class TextureLoader {
    public:
        static Texture getStageTexture(Jam::Domain::Stage::StageType type);
        static void preloadStageTextures();
        static void clearTextureCache();
        static bool loadCustomTexture(Jam::Domain::Stage::StageType type, const FilePath& filePath);
        
        // 背景テクスチャ関連（名前ベース）
        static Optional<Texture> getTexture(const String& textureName);
        static void preloadBackgroundTextures();
        static bool loadBackgroundTexture(const String& name, const FilePath& filePath);
        
    private:
        static HashTable<Jam::Domain::Stage::StageType, Texture> s_stageTextures;
        static HashTable<String, Texture> s_backgroundTextures;
        static bool s_initialized;
        
        static FilePath getDefaultTexturePath(Jam::Domain::Stage::StageType type);
        static FilePath getDefaultBackgroundTexturePath(const String& textureName);
        static void initialize();
    };

    // === 実装部分 ===

    // 静的メンバの定義
    HashTable<Jam::Domain::Stage::StageType, Texture> TextureLoader::s_stageTextures;
    HashTable<String, Texture> TextureLoader::s_backgroundTextures;
    bool TextureLoader::s_initialized = false;

    Texture TextureLoader::getStageTexture(Jam::Domain::Stage::StageType type) {
        if (!s_initialized) {
            initialize();
        }
        
        // キャッシュから検索
        if (s_stageTextures.contains(type)) {
            return s_stageTextures[type];
        }
        
        // 見つからない場合は動的に読み込み
        FilePath texturePath = getDefaultTexturePath(type);
        
        if (FileSystem::Exists(texturePath)) {
            Texture texture(texturePath);
            if (texture) {
                s_stageTextures[type] = texture;
                return texture;
            }
        }
        
        return Texture{};
    }

    void TextureLoader::preloadStageTextures() {
        Array<Jam::Domain::Stage::StageType> stageTypes = {
			Jam::Domain::Stage::StageType::Normal,
            Jam::Domain::Stage::StageType::MovingPlatform,
            Jam::Domain::Stage::StageType::Hazard,
            Jam::Domain::Stage::StageType::Trigger,
            Jam::Domain::Stage::StageType::Breakable,
        };
        
        for (auto type : stageTypes) {
            getStageTexture(type);
        }
    }

    void TextureLoader::clearTextureCache() {
        s_stageTextures.clear();
        s_initialized = false;
    }

    bool TextureLoader::loadCustomTexture(Jam::Domain::Stage::StageType type, const FilePath& filePath) {
        if (!FileSystem::Exists(filePath)) {
            return false;
        }
        
        Texture texture(filePath);
        s_stageTextures[type] = texture;
        return true;
    }

    FilePath TextureLoader::getDefaultTexturePath(Jam::Domain::Stage::StageType type) {
        const FilePath basePath = U"../Assets/Stage/";
        
        switch (type) {
        case Jam::Domain::Stage::StageType::Normal:
            return basePath + U"White_Test.png";
        case Jam::Domain::Stage::StageType::MovingPlatform:
            return basePath + U"moving_platform.png";
        case Jam::Domain::Stage::StageType::Hazard:
            return basePath + U"White_Test.png";
        case Jam::Domain::Stage::StageType::Trigger:
            return basePath + U"White_Test.png";
        case Jam::Domain::Stage::StageType::Breakable:
            return basePath + U"White_Test.png";
        default:
            return basePath + U"White_Test.png";
        }
    }

    // 背景テクスチャの取得（名前ベース）
    Optional<Texture> TextureLoader::getTexture(const String& textureName) {
        if (!s_initialized) {
            initialize();
        }
        
        // キャッシュから検索
        if (s_backgroundTextures.contains(textureName)) {
            return s_backgroundTextures[textureName];
        }
        
        // 見つからない場合は動的に読み込み
        FilePath texturePath = getDefaultBackgroundTexturePath(textureName);
        
        if (FileSystem::Exists(texturePath)) {
            Texture texture(texturePath);
            if (texture) {
                s_backgroundTextures[textureName] = texture;
                return texture;
            } else {
                return none;
            }
        } else {
            return none; // Optional<Texture>のnone
        }
    }

    void TextureLoader::preloadBackgroundTextures() {
        Array<String> backgroundNames = {
            U"BG",
            U"BG1",
            U"BG2"
        };
        
        for (const auto& name : backgroundNames) {
            getTexture(name);
        }
    }

    bool TextureLoader::loadBackgroundTexture(const String& name, const FilePath& filePath) {
        if (!FileSystem::Exists(filePath)) {
            return false;
        }
        
        Texture texture(filePath);
        s_backgroundTextures[name] = texture;
        return true;
    }

    FilePath TextureLoader::getDefaultBackgroundTexturePath(const String& textureName) {
        const FilePath basePath = U"../Assets/Stage/BG/";
        
        if (textureName == U"BG") {
            return basePath + U"BG.png";
        }
        if (textureName == U"BG1") {
            return basePath + U"BG_Back1.png";
        }
        if (textureName == U"BG2") {
            return basePath + U"BG_Back2.png";
        }
        
        return basePath + textureName + U".png";
    }

    void TextureLoader::initialize() {
        if (s_initialized) return;
        s_initialized = true;
    }
}
