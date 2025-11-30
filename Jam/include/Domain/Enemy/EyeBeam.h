#pragma once
#include "Domain/Enemy/EnemyBase.h"
#include "Domain/Events/GameEvents.h"
#include "Domain/Physics/ICollisionListener.h"
#include "Domain/Physics/PhysicsBodyID.h"
#include "Domain/Physics/IPhysicsBody.h"
#include "Infrastructure/FactoryServiceLocator.h"
#include "Infrastructure/IPhysicsBodyFactory.h"

namespace Jam::Domain::Enemy
{
	// 長いビーム
	class EyeBeam : public Jam::Domain::Physics::ICollisionListener
	{
	private:
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		Jam::Domain::Physics::PhysicsBodyID m_playerId;
		Jam::Domain::Events::GameEventQueue& m_eventQueue;
		double m_power;
		Vec2 m_size;
		double m_angle;  // 角度を追加
		Texture m_texture;

	public:
		EyeBeam(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
				Jam::Domain::Physics::PhysicsBodyID playerId,
				Jam::Domain::Events::GameEventQueue& eventQueue,
				double power,
				Vec2 size,
				double angle)  // 角度を受け取る
			: m_body(body), m_playerId(playerId), m_eventQueue(eventQueue),
			m_power(power), m_size(size), m_angle(angle)
		{
			m_texture = Texture{ Resource(U"Assets/Enemy/Eye/beam.png") };
		}

		void draw()
		{
			// ビームの位置を取得
			Vec2 pos = m_body->getPosition();

			// 回転を考慮した矩形を描画
			RectF rect(pos - m_size / 2, m_size);

			if (m_texture)
			{
				// テクスチャがある場合はテクスチャを描画
				m_texture.resized(m_size)
					.rotated(m_angle)
					.drawAt(pos, ColorF(1.0, 1.0, 1.0, 0.8));  // 少し透明にする場合
			}
			else
			{
				// テクスチャがない場合はデフォルトの描画
				rect.rotatedAt(pos, m_angle)
					.draw(ColorF(1.0, 0.0, 0.0, 0.5));  // 半透明の赤

				// 枠線を描画
				rect.rotatedAt(pos, m_angle)
					.drawFrame(2.0, ColorF(1.0, 0.0, 0.0));  // 赤い枠線
			}
		}

		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override
		{
			switch (other->getLayer())
			{
			case Jam::Domain::Physics::PhysicsLayer::Player:
				m_eventQueue.push(Events::PlayerDamagedEvent{
					m_body->getID(),
					m_playerId,
					DamageInfo {
						m_power,
						m_body->getPosition(),
						(Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory()->getBody(m_playerId)->getPosition() - m_body->getPosition()).normalized(),
						true,
						false
					},
					0.0,
					0.3,
					15.0
				});
				break;
			default:
				break;
			}
		}

		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override {};
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override {};
	};
}
