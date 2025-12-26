#pragma once

namespace Jam::Domain::Player
{
	// プレイヤーの状態異常種類
	enum class StatusAilmentType
	{
		None = 0,
		Poison, // 毒：一定時間毎にダメージ
		Paralysis, // しびれ: 移動・ジャンプが鈍くなる
	};

	struct StatusAilment
	{
		StatusAilmentType type = StatusAilmentType::None;
		bool active = false;
		double duration = 0.0;      // 効果時間
		double tickInterval = 0.0;  // 効果適用間隔（毒ダメージ）
		double tickTimer = 0.0;     // 経過時間
		double power = 0.0;    // 効果量（ダメージ量、鈍化率）

		void clear()
		{
			type = StatusAilmentType::None;
			active = false;
			duration = 0.0;
			tickInterval = 0.0;
			tickTimer = 0.0;
			power = 0.0;
		}
	};
}
