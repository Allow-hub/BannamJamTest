#pragma once
#include "../../Physics/IPhysicsBody.h"
#include "../../Physics/ICollisionListener.h"
#include "../../../Presentation/AnimatorLoader.h"
#include "../../../Presentation/Animator.h"

namespace Jam::Domain::Player
{
	// TODO : Animatorが聞かない
	// プレイヤーの手を離れるオブジェクトは独立したオブジェクトのファクトリーを作ってそれをListにしてupdate
	//する形にしないとPlayerがやられたらBombも消えることになる
	class BombEntity : public Jam::Domain::Physics::ICollisionListener
	{
	private:
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		double m_timer = 0.0;
		double m_lifeTime = 3.0; // 爆発までの時間
		bool m_exploded = false;
		Jam::Presentation::Animator anim;

	public:
		BombEntity(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body)
			: m_body(body)
		{
			Jam::Presentation::AnimatorLoader::LoadAnimatorFromJSON(anim, U"../Assets/Player/Skill/Bomb/playerSkill_bomb.json");
			anim.AddCondition({ { {U"isExploding", true} }, U"ExplodeBomb", 1 });
			anim.AddCondition({ { {U"isExploding", false} }, U"NormalBomb", 0 });
			anim.SetBool(U"isExploding", false);
		}
		
		bool isAlive() const { return !m_exploded; }

		void update(double deltaTime)
		{
			if (m_exploded) return;
			anim.Update(deltaTime);
			//anim.debug();
			m_timer += deltaTime;

			// 時間経過で爆発
			if (m_timer >= m_lifeTime)
			{
				explode();
			}
		}

		void draw() const
		{
			if (!m_exploded)
			{
				anim.Draw(m_body->getPosition());
				m_body->drawFrame(3.0, Palette::Darkcyan); // 毎フレーム描画
			}
		}

		void explode()
		{
			if (m_exploded) return;
			m_exploded = true;
			//Print << U"💥 Bomb exploded!";

		}

		s3d::Vec2 getPos() const
		{
			return m_body->getPosition();
		}
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> getBody()
		{
			return m_body;
		}

		// ICollisionListener
		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override
		{
			if (other->getLayer() == Jam::Domain::Physics::PhysicsLayer::Enemy)
			{
				Print << U"Bomb hit Enemy";
				explode();
			}
		}

		//特に処理なし
		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override{}
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override{}
	};
}
