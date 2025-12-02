#pragma once
#include "Domain/Player/Player.h"
#include "Domain/IInputService.h"
#include "Presentation/PlayerManager.h"
#include "UseCase/AttackProcessor.h"
#include <Siv3D.hpp>

namespace Jam::UseCase
{
	//　プレイヤーの更新
	class PlayerService
	{
	private:
		std::shared_ptr<Domain::Player::Player> m_player;
		Domain::IInputService& m_input;
		Jam::Presentation::PlayerManager& m_manager;

		Timer damageTimer{ 1.0s };
		void onPlayerDamaged();

		// クリア演出が既に再生されたかのフラグ
		bool m_clearAnimationPlayed = false;

		// クリアアニメーション遅延関連
		bool m_clearAnimPending = false;        // 再生待ち状態か
		double m_clearAnimDelayTimer = 0.0;     // 経過時間
		double m_clearAnimDelay = 1.0;          // 遅延秒数

	public:
		PlayerService(const std::shared_ptr<Domain::Player::Player>& player,
					  Domain::IInputService& input,
					  Jam::Presentation::PlayerManager& manager);

		void update(double deltaTime);

		// ゴール通知を受け取ったときの即時処理
		void onGoalReached(const Vec2& goalPos);

		std::shared_ptr<Domain::Player::Player> getPlayer() const;
	};
}
