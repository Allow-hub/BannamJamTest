#pragma once
#include <Siv3D.hpp>
#include "Presentation/TextureManager.h"
#include "Presentation/AnimatorLoader.h"

namespace Jam::Presentation
{
	// シーンごとのリソースグループ
	enum class ResourceGroup
	{
		Common,         // UI等の共通リソース
		PlayerIdle,
		PlayerWalk,
		PlayerJump,
		PlayerChoker,
		Enemy,          // 敵キャラ
		Stage,          // ステージ背景等
	};

	class ResourceManager
	{
	private:
		static inline std::unordered_map<ResourceGroup, Array<String>> s_groupedPaths;
		static inline std::unordered_map<ResourceGroup, bool> s_initialized;

	public:

		// プレイヤー：待機アニメーション
		// プレイヤー：待機アニメーション
		static void initPlayerIdle()
		{
			registerGroup(ResourceGroup::PlayerIdle, {
				U"Assets/Player/Idle/idle_00.png",
				U"Assets/Player/Idle/idle_01.png",
				U"Assets/Player/Idle/idle_02.png",
				U"Assets/Player/Idle/idle_03.png",
				U"Assets/Player/Idle/idle_04.png",
				U"Assets/Player/Idle/idle_05.png",
				U"Assets/Player/Idle/idle_06.png",
				U"Assets/Player/Idle/idle_07.png",
				U"Assets/Player/Idle/idle_08.png",
				U"Assets/Player/Idle/idle_09.png",
				U"Assets/Player/Idle/idle_10.png",
				U"Assets/Player/Idle/idle_11.png",
				U"Assets/Player/Idle/idle_12.png",
				U"Assets/Player/Idle/idle_13.png",
				U"Assets/Player/Idle/idle_14.png",
				U"Assets/Player/Idle/idle_15.png",
				U"Assets/Player/Idle/idle_16.png",
				U"Assets/Player/Idle/idle_17.png",
				U"Assets/Player/Idle/idle_18.png",
				U"Assets/Player/Idle/idle_19.png",
				U"Assets/Player/Idle/idle_20.png",
				U"Assets/Player/Idle/idle_21.png",
				U"Assets/Player/Idle/idle_22.png",
				U"Assets/Player/Idle/idle_23.png",
				U"Assets/Player/Idle/idle_24.png",
				U"Assets/Player/Idle/idle_25.png",
				U"Assets/Player/Idle/idle_26.png",
				U"Assets/Player/Idle/idle_27.png",
				U"Assets/Player/Idle/idle_28.png",
				U"Assets/Player/Idle/idle_29.png",
			});
		}

		// プレイヤー：走りアニメーション
		static void initPlayerWalk()
		{
			registerGroup(ResourceGroup::PlayerWalk, {
				U"Assets/Player/Walk/walk_00.png",
				U"Assets/Player/Walk/walk_01.png",
				U"Assets/Player/Walk/walk_02.png",
				U"Assets/Player/Walk/walk_03.png",
				U"Assets/Player/Walk/walk_04.png",
				U"Assets/Player/Walk/walk_05.png",
				U"Assets/Player/Walk/walk_06.png",
				U"Assets/Player/Walk/walk_07.png",
				U"Assets/Player/Walk/walk_08.png",
				U"Assets/Player/Walk/walk_09.png",
				U"Assets/Player/Walk/walk_10.png",
				U"Assets/Player/Walk/walk_11.png",
				U"Assets/Player/Walk/walk_12.png",
				U"Assets/Player/Walk/walk_13.png",
				U"Assets/Player/Walk/walk_14.png",
				U"Assets/Player/Walk/walk_15.png",
				U"Assets/Player/Walk/walk_16.png",
				U"Assets/Player/Walk/walk_17.png",
				U"Assets/Player/Walk/walk_18.png",
				U"Assets/Player/Walk/walk_19.png",
				U"Assets/Player/Walk/walk_20.png",
				U"Assets/Player/Walk/walk_21.png",
				U"Assets/Player/Walk/walk_22.png",
				U"Assets/Player/Walk/walk_23.png",
				U"Assets/Player/Walk/walk_24.png",
				U"Assets/Player/Walk/walk_25.png",
				U"Assets/Player/Walk/walk_26.png",
				U"Assets/Player/Walk/walk_27.png",
				U"Assets/Player/Walk/walk_28.png",
				U"Assets/Player/Walk/walk_29.png",
			});
		}

		// プレイヤー：ジャンプアニメーション
		static void initPlayerJump()
		{
			registerGroup(ResourceGroup::PlayerJump, {
				U"Assets/Player/Jump/jump_00.png",
				U"Assets/Player/Jump/jump_01.png",
				U"Assets/Player/Jump/jump_02.png",
				U"Assets/Player/Jump/jump_03.png",
				U"Assets/Player/Jump/jump_04.png",
				U"Assets/Player/Jump/jump_05.png",
				U"Assets/Player/Jump/jump_06.png",
				U"Assets/Player/Jump/jump_07.png",
				U"Assets/Player/Jump/jump_08.png",
				U"Assets/Player/Jump/jump_09.png",
				U"Assets/Player/Jump/jump_10.png",
				U"Assets/Player/Jump/jump_11.png",
				U"Assets/Player/Jump/jump_12.png",
				U"Assets/Player/Jump/jump_13.png",
				U"Assets/Player/Jump/jump_14.png",
				U"Assets/Player/Jump/jump_15.png",
				U"Assets/Player/Jump/jump_16.png",
				U"Assets/Player/Jump/jump_17.png",
				U"Assets/Player/Jump/jump_18.png",
				U"Assets/Player/Jump/jump_19.png",
				U"Assets/Player/Jump/jump_20.png",
				U"Assets/Player/Jump/jump_21.png",
				U"Assets/Player/Jump/jump_22.png",
				U"Assets/Player/Jump/jump_23.png",
				U"Assets/Player/Jump/jump_24.png",
				U"Assets/Player/Jump/jump_25.png",
				U"Assets/Player/Jump/jump_26.png",
				U"Assets/Player/Jump/jump_27.png",
				U"Assets/Player/Jump/jump_28.png",
				U"Assets/Player/Jump/jump_29.png",
			});
		}

		// プレイヤー：チョーカー投げアニメーション
		static void initPlayerChoker()
		{
			registerGroup(ResourceGroup::PlayerChoker, {
				U"Assets/Player/ChokerThrow/choker_throw_00.png",
				U"Assets/Player/ChokerThrow/choker_throw_01.png",
				U"Assets/Player/ChokerThrow/choker_throw_02.png",
				U"Assets/Player/ChokerThrow/choker_throw_03.png",
				U"Assets/Player/ChokerThrow/choker_throw_04.png",
				U"Assets/Player/ChokerThrow/choker_throw_05.png",
				U"Assets/Player/ChokerThrow/choker_throw_06.png",
				U"Assets/Player/ChokerThrow/choker_throw_07.png",
				U"Assets/Player/ChokerThrow/choker_throw_08.png",
				U"Assets/Player/ChokerThrow/choker_throw_09.png",
				U"Assets/Player/ChokerThrow/choker_throw_10.png",
				U"Assets/Player/ChokerThrow/choker_throw_11.png",
				U"Assets/Player/ChokerThrow/choker_throw_12.png",
				U"Assets/Player/ChokerThrow/choker_throw_13.png",
				U"Assets/Player/ChokerThrow/choker_throw_14.png",
				U"Assets/Player/ChokerThrow/choker_throw_15.png",
				U"Assets/Player/ChokerThrow/choker_throw_16.png",
				U"Assets/Player/ChokerThrow/choker_throw_17.png",
				U"Assets/Player/ChokerThrow/choker_throw_18.png",
				U"Assets/Player/ChokerThrow/choker_throw_19.png",
				U"Assets/Player/ChokerThrow/choker_throw_20.png",
				U"Assets/Player/ChokerThrow/choker_throw_21.png",
				U"Assets/Player/ChokerThrow/choker_throw_22.png",
				U"Assets/Player/ChokerThrow/choker_throw_23.png",
				U"Assets/Player/ChokerThrow/choker_throw_24.png",
				U"Assets/Player/ChokerThrow/choker_throw_25.png",
				U"Assets/Player/ChokerThrow/choker_throw_26.png",
				U"Assets/Player/ChokerThrow/choker_throw_27.png",
				U"Assets/Player/ChokerThrow/choker_throw_28.png",
				U"Assets/Player/ChokerThrow/choker_throw_29.png",
			});
		}

		// ========== 基本機能 ==========

		// リソースグループの登録
		static void registerGroup(ResourceGroup group, const Array<String>& paths)
		{
			s_groupedPaths[group] = paths;
			s_initialized[group] = false;
		}

		// 指定グループのリソースを読み込み
		static void loadGroup(ResourceGroup group)
		{
			if (!s_groupedPaths.contains(group)) return;
			if (s_initialized[group]) return; // すでに読み込み済み


			for (const auto& path : s_groupedPaths[group])
			{
				TextureManager::Load(path);
			}

			s_initialized[group] = true;
		}

		// 複数グループを一括ロード
		static void loadGroups(const Array<ResourceGroup>& groups)
		{
			for (const auto& group : groups)
			{
				loadGroup(group);
			}
		}

		// リソースグループをクリア（メモリ解放したい場合）
		static void Clear()
		{
			s_groupedPaths.clear();
			s_initialized.clear();
			TextureManager::Clear();
		}
	};
}
