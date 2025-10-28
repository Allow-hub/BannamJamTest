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

	struct StageData
	{
		Vec2 respawnPosition;  // 落下後のリスタート位置
		double fallLimitY;     // 落下判定Y位置
		std::array<Vec2, 3> flagmentMemoryPos;
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
		inline static StageData m_currentStageData;
		inline static bool m_isDied = false;
		inline static bool m_isPause = false;

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

		static StageData getStageData(StageName stage)
		{
			switch (stage)
			{
			case StageName::Stage1_1:
				return StageData{ Vec2(50, -5), 2000.0, {Vec2(200, -100),Vec2(800, -300),Vec2(1300, -250)} }; // respawn, fallLimitY
			case StageName::Stage1_2:
				return StageData{ Vec2(50, -5), 2000.0, {Vec2(200, -100),Vec2(800, -300),Vec2(1300, -250)} };
			case StageName::Stage1_3:
				return StageData{ Vec2(50, 300), 550.0, {Vec2(200, -100),Vec2(800, -300),Vec2(1300, -250)} };
			default:
				return StageData{ Vec2(50, 0), 1000.0, {Vec2(200, -100),Vec2(800, -300),Vec2(1300, -250)} };
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

		static bool getDied()
		{
			return m_isDied;
		}

		static void setDied(bool b)
		{
			m_isDied = b;
		}

		static bool getPause()
		{
			return m_isPause;
		}

		static void setPause(bool b)
		{
			m_isPause = b;
		}

		static void reset()
		{
			setDied(false);
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

		static void setCurrentStageData(const StageData& data)
		{
			m_currentStageData = data;
		}

		static const StageData& getCurrentStageData()
		{
			return m_currentStageData;
		}

		static void setRespawnPosition(const Vec2& pos)
		{
			m_currentStageData.respawnPosition = pos;
		}

		static void setFallLimitY(double y)
		{
			m_currentStageData.fallLimitY = y;
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
