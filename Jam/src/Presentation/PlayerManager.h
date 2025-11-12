#pragma once
#include <Siv3D.hpp>
#include "../Domain/Player/Player.h"
#include "Animator.h"
#include "AnimatorLoader.h"

namespace Jam::Presentation
{
	//プレイヤーの描画をメインに担当
    class PlayerManager
    {
    private:
		Animator anim;
        std::shared_ptr<Domain::Player::Player> m_player;
        Texture m_texture; // プレイヤー画像

    public:
        PlayerManager(const std::shared_ptr<Domain::Player::Player>& player)
            : m_player(player)
        {
			// JSON からクリップをロード
			Jam::Presentation::AnimatorLoader::loadAnimatorFromJSON(anim, U"Assets/Player/player_animation_dummy.json");
			// 条件を設定
			anim.setDefaultClip(U"Idle");
			anim.addCondition({ { {U"isWalking", true} }, U"Walk", 1 });
			anim.addCondition({ { {U"isRunning", true} }, U"Run", 2 });
			anim.addCondition({ { {U"isJumping", true} }, U"Jump", 3 });
			anim.addCondition({ { {U"isChokerThrow", true} }, U"Choker_Throw", 4 });

			anim.setBool(U"isRunning", false);
			anim.setBool(U"isJumping", false);
			anim.setBool(U"isChokerThrows", false);
		}

		void setAnim(String name,bool b)
		{
			anim.setBool(name, b);
		}

		void update()
		{
			anim.update(Scene::DeltaTime());
		}

		void setFacingLeft(bool b) {
			anim.setFacingLeft(b);
		}

        void draw() const
        {
            Vec2 pos = m_player->getPosition();
			anim.draw(pos);
			m_player->draw();
        }
    };
}
