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
        
    private:
        static HashTable<Jam::Domain::Stage::StageType, Texture> s_stageTextures;
        static bool s_initialized;
        
        static FilePath getDefaultTexturePath(Jam::Domain::Stage::StageType type);
        static void initialize();
    };

    // === 実装部分 ===

    // 静的メンバの定義
    HashTable<Jam::Domain::Stage::StageType, Texture> TextureLoader::s_stageTextures;
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
            s_stageTextures[type] = texture;
            Console << U"[TextureLoader] Loaded texture: " << texturePath;
            return texture;
        } else {
            Console << U"[TextureLoader] ❌ Texture not found: " << texturePath;
            return Texture{}; // 空のテクスチャを返す
        }
    }

    void TextureLoader::preloadStageTextures() {
        Console << U"[TextureLoader] Preloading stage textures...";
        
        Array<Jam::Domain::Stage::StageType> stageTypes = {
            Jam::Domain::Stage::StageType::Normal,
            Jam::Domain::Stage::StageType::Platform,
            Jam::Domain::Stage::StageType::Hazard,
            Jam::Domain::Stage::StageType::Trigger,
            Jam::Domain::Stage::StageType::Breakable,
            Jam::Domain::Stage::StageType::MovingPlatform
        };
        
        for (auto type : stageTypes) {
            getStageTexture(type);
        }
        
        Console << U"[TextureLoader] ✅ Preloading completed (" << s_stageTextures.size() << U" textures)";
    }

    void TextureLoader::clearTextureCache() {
        s_stageTextures.clear();
        s_initialized = false;
        Console << U"[TextureLoader] Texture cache cleared";
    }

    bool TextureLoader::loadCustomTexture(Jam::Domain::Stage::StageType type, const FilePath& filePath) {
        if (!FileSystem::Exists(filePath)) {
            Console << U"[TextureLoader] ❌ Custom texture not found: " << filePath;
            return false;
        }
        
        Texture texture(filePath);
        s_stageTextures[type] = texture;
        Console << U"[TextureLoader] ✅ Custom texture loaded: " << filePath;
        return true;
    }

    FilePath TextureLoader::getDefaultTexturePath(Jam::Domain::Stage::StageType type) {
        const FilePath basePath = U"../Assets/Stage/";
        
        switch (type) {
        case Jam::Domain::Stage::StageType::Normal:
            return basePath + U"Normal_Test.png";
        case Jam::Domain::Stage::StageType::Platform:
            return basePath + U"Platform_Test.png";
        case Jam::Domain::Stage::StageType::Hazard:
            return basePath + U"hazard.png";
        case Jam::Domain::Stage::StageType::Trigger:
            return basePath + U"trigger.png";
        case Jam::Domain::Stage::StageType::Breakable:
            return basePath + U"breakable.png";
        case Jam::Domain::Stage::StageType::MovingPlatform:
            return basePath + U"moving_platform.png";
        default:
            return basePath + U"default.png";
        }
    }

    void TextureLoader::initialize() {
        if (s_initialized) return;
        s_initialized = true;
        Console << U"[TextureLoader] Initialized";
    }
}
