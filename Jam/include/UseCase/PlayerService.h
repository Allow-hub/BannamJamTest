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

	public:
		PlayerService(const std::shared_ptr<Domain::Player::Player>& player,
					  Domain::IInputService& input,
					  Jam::Presentation::PlayerManager& manager);

		void update(double deltaTime);

		std::shared_ptr<Domain::Player::Player> getPlayer() const;
	};
}
