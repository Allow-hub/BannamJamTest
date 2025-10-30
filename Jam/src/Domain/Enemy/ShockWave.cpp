#include "ShockWave.h"
#include "../../Infrastructure/FactoryServiceLocator.h"
#include "../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../Infrastructure/PhysicsFilterManager.h"

using namespace Jam::Domain::Physics;
using namespace Jam::Infrastructure::Locator;

namespace Jam::Domain::Enemy
{
	ShockWave::ShockWave(const Vec2& pos, double duration, Vec2 startRect, Vec2 endRect, Jam::Domain::Physics::PhysicsBodyID playerId, Jam::Domain::Events::GameEventQueue& queue)
		: m_duration(duration)
		, m_startRect(startRect)
		, m_endRect(endRect)
		, m_playerId(playerId)
		, m_eventQueue(queue)
		, m_lastRegenTime(0.0)
	{
		m_body = FactoryServiceLocator::instance()
			.getPhysicsFactory()->createRectSensor(pos, m_startRect);
		m_body->setLayer(PhysicsLayer::Enemy);
		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::EnemyWeapon);
		m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Static);
		m_body->setGravityScale(0);
		m_hittedPlayer = false;
		// テクスチャ複数枚を読み込む
		for (int i = 0; i < 5; ++i)
		{
			m_textures.emplace_back(Texture(U"../Assets/Enemy/Boss1_3/Shockwave/shockwave_0" + Format(i) + U".png"));
		}
	}

	void ShockWave::update(double dt)
	{
		m_timer += dt;

		if (m_timer >= m_duration)
		{
			m_finished = true;
			m_body.reset();
			return;
		}

		double t = m_timer / (m_duration * 0.35);
		Vec2 currentRect = Math::Lerp(m_startRect, m_endRect, t);

		if (m_timer - m_lastRegenTime >= 0.1)
		{
			m_lastRegenTime = m_timer;
			auto pos = m_body->getPosition();

			m_body.reset();
			m_body = FactoryServiceLocator::instance()
				.getPhysicsFactory()->createRectSensor(pos, currentRect);
			m_body->setCollisionListener(shared_from_this());
			m_body->setLayer(PhysicsLayer::Enemy);
			m_body->setFilter(Jam::Infrastructure::PhysicsFilter::EnemyWeapon);
			m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Static);
			m_body->setGravityScale(0);
		}

		// アニメーションフレーム更新（0.01秒ごとに切り替え）
		m_animTimer += dt;
		if (m_animTimer >= 0.01)
		{
			m_animTimer = 0.0;
			m_frameIndex = (m_frameIndex + 1) % m_textures.size();
		}
	}

	void ShockWave::draw() const
	{
		if (!m_body) return;
		//m_body->drawFrame(5.0, ColorF(0.0, 1.0, 0.0, 0.9)); // デバッグ用に当たり判定を表示

		// 伸びる速度倍率
		constexpr double growthSpeed = 4.0; // 小さいほどゆっくり、大きいほど早く

		// 進行率
		double t = (m_timer / m_duration) * growthSpeed;

		// 一定以上は伸びを止める
		t = Clamp(t, 0.0, 1.0);

		// 1まで達したら「完全に伸びきった状態」を維持
		Vec2 currentRect = Math::Lerp(m_startRect, m_endRect, t >= 1.0 ? 1.0 : t);
		Vec2 pos = m_body->getPosition();
		pos.y -= 50;
		const Texture& tex = m_textures[m_frameIndex];

		// 画像の切り取り幅（1.0を超えても最大に固定）
		int displayWidth = static_cast<int>(tex.width() * Min(t, 1.0));
		if (displayWidth <= 0) return;

		// スケーリング
		Vec2 scale = currentRect / tex.size();
		scale.y *= 2.5;

		// 描画（完全に伸びきった後は幅固定・アニメのみ）
		tex(0, 0, displayWidth, tex.height())
			.scaled(scale)
			.drawAt(pos, ColorF(1.0, 1.0, 1.0, 1.0));
	}


	void ShockWave::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		switch (other->getLayer())
		{
		case Jam::Domain::Physics::PhysicsLayer::Player:
			if (m_hittedPlayer) return;//一回のみ
			m_eventQueue.push(Events::PlayerDamagedEvent{
				m_body->getID(),
				m_playerId,
				DamageInfo {
				m_attackPower,
				m_body->getPosition(),
				(Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory()->getBody(m_playerId)->getPosition()
					- m_body->getPosition()).normalized(),
				true,
				false
				}
				,0.0
				,0.3
				,15.0
			});
			m_hittedPlayer = true;	
			break;
		}
	}

	void ShockWave::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
	}

	void ShockWave::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
	}
}
