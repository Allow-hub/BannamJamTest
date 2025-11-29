#pragma once
#include <Siv3D.hpp>
#include <memory>
#include "../Domain/Player/Player.h"
#include "Animator.h"

namespace Jam::Presentation
{
	// プレイヤーの描画をメインに担当
	class PlayerManager
	{
	private:
		Animator anim;
		std::shared_ptr<Domain::Player::Player> m_player;
		Texture m_texture; // プレイヤー画像

	public:
		PlayerManager(const std::shared_ptr<Domain::Player::Player>& player);

		void setAnim(String name, bool b);
		void update();
		void setFacingLeft(bool b);
		void draw() const;
	};
}
