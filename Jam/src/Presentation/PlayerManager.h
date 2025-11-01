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
			Jam::Presentation::AnimatorLoader::LoadAnimatorFromJSON(anim, U"Assets/Player/player_animation.json");
			// 条件を設定
			anim.SetDefaultClip(U"Idle");
			anim.AddCondition({ { {U"isWalking", true} }, U"Walk", 1 });
			anim.AddCondition({ { {U"isRunning", true} }, U"Run", 2 });
			anim.AddCondition({ { {U"isJumping", true} }, U"Jump", 3 });
			anim.AddCondition({ { {U"isChokerThrow", true} }, U"Choker_Throw", 4 });

			anim.SetBool(U"isRunning", false);
			anim.SetBool(U"isJumping", false);
			anim.SetBool(U"isChokerThrows", false);
		}

		void setAnim(String name,bool b)
		{
			anim.SetBool(name, b);
		}

		void update()
		{
			anim.Update(Scene::DeltaTime());
		}

		void setFacingLeft(bool b) {
			anim.SetFacingLeft(b);
		}

        void draw() const
        {
            Vec2 pos = m_player->getPosition();
			anim.Draw(pos);
			m_player->draw();
        }
    };
}
