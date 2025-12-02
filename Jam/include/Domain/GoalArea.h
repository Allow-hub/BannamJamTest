#pragma once
#include "Domain/IIndependentObject.h"
#include "Domain/Physics/IPhysicsBody.h"
#include "Domain/Physics/ICollisionListener.h"
#include "Presentation/AudioService.h"
#include "UseCase/CameraEvent.h"
#include "Foundation/CoreManager.h"
#include "UseCase/EffectEvents.h"

namespace Jam::Domain
{
	class GoalArea :public Jam::Domain::IIndependentObject, public Jam::Domain::Physics::ICollisionListener
	{
	private:
		bool m_collected = false;
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		std::function<void()> m_onClear;
		std::function<void()> m_onPlayerClear;   // プレイヤーにゴールを通知する即時コールバック
		Texture m_texture;
		double m_scale = 0.3;
		Vec2 m_offset = Vec2(100, 100);
		Jam::UseCase::EffectEventQueue& m_effectQueue;
		Jam::UseCase::CameraEventQueue& m_cameraQueue;

		double m_elapsed = 0.0;      // 経過時間（秒）
		const double m_waitTime = 3.0; // 待機時間（秒）
		enum GoalState
		{
			Idle = 0,
			Cleared,
			End
		};
		GoalState m_state = GoalState::Idle;

	public:
		GoalArea(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
		         std::function<void()> onClear,
		         Jam::UseCase::EffectEventQueue& effectQueue,
		         Jam::UseCase::CameraEventQueue& cameraQueue)
			: m_body(body), m_onClear(onClear), m_effectQueue(effectQueue), m_cameraQueue(cameraQueue)
		{
			m_texture = Texture(Resource(U"Assets/Stage/goal.png"));
		}

		// プレイヤーへ即時通知コールバックを設定
		void setOnPlayerClear(std::function<void()> cb)
		{
			m_onPlayerClear = std::move(cb);
		}

		void update(double deltaTime)override
		{
			switch (m_state)
			{
			case Jam::Domain::GoalArea::Idle:
				break;
			case Jam::Domain::GoalArea::Cleared:
				m_elapsed += deltaTime;
				if (m_elapsed >= m_waitTime)
				{
					if (m_onClear)
					{
						// ゴールエフェクト発生
						m_effectQueue.push(Jam::UseCase::StarEffectEvent{
							.position = m_body->getPosition(),
							.hue = 50.0,
							.starCount = 8,
							.radius = 100.0,
							.duration = 0.6,
							.starSize = 48.0,
							.gravity = {0, 160},
							.hueVariation = 12.0
						});
						m_onClear();
					}
				}
				break;
			case Jam::Domain::GoalArea::End:
				break;
			default:
				break;
			}
		}

		void draw() const override
		{
			m_texture.scaled(m_scale).draw(m_body->getPosition() - m_offset);
		}

		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override
		{
			switch (other->getLayer())
			{
			case Jam::Domain::Physics::PhysicsLayer::Player:
				if (m_collected) return;

				// ゴールエフェクト発生
				m_effectQueue.push(Jam::UseCase::StarEffectEvent{
					.position = m_body->getPosition(),
					.hue = 50.0,
					.starCount = 8,
					.radius = 100.0,
					.duration = 0.6,
					.starSize = 48.0,
					.gravity = {0, 160},
					.hueVariation = 12.0
				});
				// ゴール時のSEを再生
				Jam::Presentation::AudioService::get().playOneShot(
					Jam::Presentation::AudioService::Sound::SE_Goal,
					1.0
				);

				// クリアフラグを立てる
				Jam::Foundation::CoreManager::Instance().setClear(true);
				// フラグを立てて描画停止
				m_collected = true;
				// 状態を変更して待機
				m_state = GoalState::Cleared;
				m_elapsed = 0.0;

				// カメラをゴールにフォーカス
				m_cameraQueue.push(Jam::UseCase::CameraFocusEvent{
					.target = m_body->getPosition(),
					.duration = 3.5,
					.zoom = 2.0
				});

				// プレイヤーへの通知
				if (m_onPlayerClear)
				{
					m_onPlayerClear();
				}
				break;
			default:
				break;
			}
		}

		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override {}
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override {}
	};
}
