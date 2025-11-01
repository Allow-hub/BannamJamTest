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

	struct GoalData
	{
		Vec2 position;
		Vec2 size;
	};

	struct StageData
	{
		Vec2 respawnPosition;  // 落下後のリスタート位置
		double fallLimitY;     // 落下判定Y位置
		std::array<Vec2, 3> flagmentMemoryPos;
		GoalData goalData;
	};

	class CoreManager
	{
	private:
		inline static bool m_clear;
		inline static bool m_nextStagePressed = false;
		inline static int m_flagmentMemory;
		static const int m_maxFlagment = 3;
		inline static double m_timer;
		inline static StageData m_currentStageData;
		inline static bool m_isDied = false;
		inline static bool m_isPause = false;
		inline static int m_defeatedEnemyCount = 0; // 敵撃破数を保持

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
				return StageData{ Vec2(50, -5), 2000.0,
					{Vec2(2800, -250),Vec2(12700, -900),Vec2(15450, -750)},
					GoalData{ Vec2(20650, 0), Vec2(200, 200) } };
			case StageName::Stage1_2:
				return StageData{ Vec2(50, -5), 2000.0,
					{Vec2(4050, -200),Vec2(6350, -250),Vec2(9450, 1400)},
					GoalData{ Vec2(12800,1800 ), Vec2(200, 200) } };
			case StageName::Stage1_3:
				return StageData{ Vec2(50, -5), 550.0,
					{ Vec2(-10000, -10000), Vec2(-10000, -10000), Vec2(-10000, -10000) },
					GoalData{ Vec2(-10000, 0), Vec2(200, 200) } };
			default:
				return StageData{ Vec2(50, 0), 1000.0,
					{ Vec2(200, -100), Vec2(800, -300), Vec2(1300, -250) },
					GoalData{ Vec2(200, 0), Vec2(200, 200) } };
			}
		}

		// 現在のステージを次のステージに変更
		static void goToNextStage()
		{
			StageName current = Instance().stageInfo.stageName;
			switch (current)
			{
			case StageName::Stage1_1:
				Instance().stageInfo.stageName = StageName::Stage1_2;
				Print << U"aa";
				break;
			case StageName::Stage1_2:
				Instance().stageInfo.stageName = StageName::Stage1_3;
				break;
			case StageName::Stage1_3:
				// 最後のステージの場合は変更しない or 最初に戻す場合は Stage1_1 にする
				 Instance().stageInfo.stageName = StageName::Stage1_1;
				break;
			default:
				break;
			}
		}
		static bool getNextStagePressed() { return m_nextStagePressed; }
		static void setNextStagePressed(bool b) { m_nextStagePressed = b; }

		static bool getClear() { return m_clear; }
		static void setClear(bool b) { m_clear = b; }

		static bool getDied() { return m_isDied; }
		static void setDied(bool b) { m_isDied = b; }

		static bool getPause() { return m_isPause; }
		static void setPause(bool b) { m_isPause = b; }

		static void reset()
		{
			setDied(false);
			setClear(false);
			setNextStagePressed(false);
			m_timer = 0.0;
			m_flagmentMemory = 0;
			m_defeatedEnemyCount = 0;
		}

		static int getFlagment() { return m_flagmentMemory; }
		static int getMaxFlagment() { return m_maxFlagment; }

		static void addFlagment(int amount)
		{
			if (m_flagmentMemory >= m_maxFlagment)
			{
				m_flagmentMemory = m_maxFlagment;
				return;
			}
			m_flagmentMemory += amount;
		}
		// 敵撃破数を1増やす（または指定数増やす）
		static void addDefeatedEnemy(int count = 1) { m_defeatedEnemyCount += count; }
		// 敵撃破数を取得
		static int getDefeatedEnemyCount() { return m_defeatedEnemyCount; }

		static void addTimer(double t) { m_timer += t; }
		static double getTimer() { return m_timer; }

		static void setCurrentStageData(const StageData& data) { m_currentStageData = data; }
		static const StageData& getCurrentStageData() { return m_currentStageData; }

		static void setRespawnPosition(const Vec2& pos) { m_currentStageData.respawnPosition = pos; }
		static void setFallLimitY(double y) { m_currentStageData.fallLimitY = y; }

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
