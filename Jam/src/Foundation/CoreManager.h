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
	private:
		inline static bool m_clear;
		inline static int m_flagmentMemory;
		static const int m_maxFlagment = 3;
		inline static double m_timer;

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

		static bool getClear()
		{
			return m_clear;
		}

		static void setClear(bool b)
		{
			m_clear = b;
		}

		static void reset()
		{
			setClear(false);
			m_timer = 0.0;
			m_flagmentMemory = 0;
		}

		static int getFlagment()
		{
			return m_flagmentMemory;
		}

		static int getMaxFlagment()
		{
			return m_maxFlagment;
		}

		static void addFlagment(int amount)
		{
			if (m_flagmentMemory >= m_maxFlagment)
			{
				m_flagmentMemory = m_maxFlagment;
				return;
			}
			m_flagmentMemory = amount;
		}

		static void addTimer(double t)
		{
			m_timer += t;
		}

		static double getTimer()
		{
			return m_timer;
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
