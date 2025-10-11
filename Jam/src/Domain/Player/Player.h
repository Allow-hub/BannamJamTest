#pragma once
#include <memory>
#include "../Physics/IPhysicsBody.h"

namespace Jam::Domain
{
	// プレイヤーキャラクターを表すクラス
	// 他クラスに依存しない
    class Player
    {
    public:
        explicit Player(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body);

        void update(double deltaTime);

        void moveLeft();
        void moveRight();
        void jump();
        void onGroundContact(bool grounded);

        s3d::Vec2 getPosition() const;
        bool isFacingRight() const;

    private:
        std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
        bool m_isGrounded = false;
        bool m_facingRight = true;

        double m_moveForce = 1000.0;
        double m_jumpImpulse = 10000.0;
        //double m_gravity = 9.8 * 10.0;

        //void applyGravity(double deltaTime);
        void updateState();
    };
}
