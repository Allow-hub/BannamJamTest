#pragma once
#include <Siv3D.hpp>
#include "../Domain/Player/Player.h"

namespace Jam::Presentation
{
    class PlayerManager
    {
    private:
        std::shared_ptr<Domain::Player> m_player;
        Texture m_texture; // プレイヤー画像

    public:
        PlayerManager(const std::shared_ptr<Domain::Player>& player)
            : m_player(player)
        {
            m_texture = Texture(U"Asset/Player/player_idle.png");
        }

        void draw() const
        {
            Vec2 pos = m_player->getPosition();
            m_texture.resized(64,64).drawAt(pos);
        }
    };
}
