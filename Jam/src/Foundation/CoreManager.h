// CoreManager.h
#pragma once
#include <Siv3D.hpp>

namespace Jam::Foundation
{
	enum class StageName
	{
		Stage1_1,
		Stage1_2,
		Stage1_3
	};

	//ロジックは持たない、他のクラスへの参照は持たない
	//ステージをまたいで情報をやり取りする
	class CoreManager
	{
	public:
		static CoreManager& Instance()
		{
			static CoreManager instance;
			return instance;
		}

		static s3d::String stageNameToString(StageName stage)
		{
			switch (stage)
			{
			case StageName::Stage1_1: return U"stage1_1";
			case StageName::Stage1_2: return U"stage1_2";
			case StageName::Stage1_3: return U"stage1_3";
			default: return U"UnknownStage";
			}
		}

		// シーン間共通情報（純粋データのみ）
		struct StageInfo {
			StageName stageName = StageName::Stage1_1;
		} stageInfo;

		struct AudioSetting {
			double masterVolume = 1.0;
			double bgmVolume = 1.0;
			double seVolume = 1.0;
		} audioSetting;


	private:
		CoreManager() = default;
	};
}
