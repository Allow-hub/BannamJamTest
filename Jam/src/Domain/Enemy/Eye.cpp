#include "Eye.h"
#include "EyeBeam.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"
#include "EnemyAI/AttackAI.h"
#include "../../Infrastructure/FactoryServiceLocator.h"
#include "../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../Infrastructure/PhysicsFilterManager.h"

namespace Jam::Domain::Enemy
{
	Eye::Eye(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
	, Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
	{
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;
		aiList.emplace_back(AIType::Patrol, std::make_unique<PatrolAI>());
		aiList.emplace_back(AIType::Attack, std::make_unique<AttackAI>());
		setAIList(std::move(aiList));
		m_enemyType = EnemyType::Eye;
		m_body->setGravityScale(0);
	}

	void Eye::update(double deltaTime)
	{
		if (!isAlive()) return;
		m_currentAI->update(*this, deltaTime);
	}
	void Eye::draw() const
	{
		if (m_beamListener)
			m_beamListener->draw();
	}
	void Eye::onAIEvent(EnemyAIEvent e)
	{
		switch (e)
		{
		case EnemyAIEvent::PlayerFound:
			changeAI(AIType::Attack);
			break;
		default:
			break;
		}
	}

	void Eye::onDestroy(const DamageInfo& info)
	{
		// ビームを削除
		if (m_beam)
		{
			Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()->removeBody(m_beam->getID());
			m_beam.reset();
			m_beamListener.reset();
		}
		EnemyBase::onDestroy(info);
	}

	void Eye::onAttackEnter()
	{
		attackState = AttackState::AttackStart;
		attackTimer = 0.0;
		swingTimer = 0.0;
	}

	void Eye::onAttackUpdate(double deltaTime)
	{
		attackTimer += deltaTime;

		switch (attackState)
		{
		case AttackState::AttackStart:
			attackTimer = 0.0;
			swingTimer = 0.0;
			attackState = AttackState::WaitAttack;
			m_attacked = false;
			break;

		case AttackState::WaitAttack:
			swingTimer += deltaTime;
			// 一定間隔ごとに左右に揺れる
			if (swingTimer >= swingInterval)
			{
				swingTimer = 0.0;
				if (m_rightMove)
				{
					m_body->setVelocity(Vec2::Zero());
					m_body->applyImpulse(Vec2::Right() * 1000);
					m_rightMove = false;
				}
				else {
					m_body->setVelocity(Vec2::Zero());
					m_body->applyImpulse(Vec2::Left() * 1000);
					m_rightMove = true;
				}
			}
			// 待機時間経過後、攻撃開始
			if (attackTimer >= waitDuration)
			{
				attackTimer = 0.0;
				m_body->setVelocity(Vec2::Zero()); // 攻撃前に動きを止める
				attackState = AttackState::IsAttack;
			}
			break;

		case AttackState::IsAttack:
			if (!m_attacked)
			{
				// プレイヤーの位置を取得
				auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
					.getPhysicsFactory()->getBody(m_playerId);
				Vec2 playerPos = playerBody->getPosition();
				Vec2 eyePos = m_body->getPosition();

				// プレイヤーへの方向ベクトルを計算
				Vec2 direction = (playerPos - eyePos).normalized();

				// ビームのサイズ
				Vec2 beamSize = { 800, 90 };

				// ビームの開始位置（敵の端）を計算
				// 敵のサイズを取得（仮に敵の半径を50とする。実際の値に調整してください）
				double eyeRadius = 50.0;
				Vec2 beamStartPos = eyePos + direction * eyeRadius;

				// ビームの中心位置を計算（開始位置からビームの長さの半分進んだ位置）
				Vec2 beamCenterPos = beamStartPos + direction * (beamSize.x / 2);

				// プレイヤーへの角度を計算
				double angle = std::atan2(direction.y, direction.x);

				// ビームの生成
				m_beam = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
					.getPhysicsFactory()->createRectSensor(beamCenterPos, beamSize);
				m_beam->setLayer(Jam::Domain::Physics::PhysicsLayer::Enemy);
				m_beam->setFilter(Jam::Infrastructure::PhysicsFilter::EnemyWeapon);

				m_beam->setAngle(angle);  // ビームを回転

				// ビームのコリジョンリスナーを設定
				m_beamListener = std::make_shared<EyeBeam>(
					m_beam,
					m_playerId,
					m_eventQueue,
					m_status.attackPower,  // ダメージ量
					beamSize,
					angle  // 角度も渡す
				);
				m_beam->setCollisionListener(m_beamListener);

				m_attacked = true;
			}
			// 攻撃時間経過後、終了へ
			if (attackTimer >= attackDuration)
			{
				attackTimer = 0.0;
				attackState = AttackState::EndAttack;
			}
			break;

		case AttackState::EndAttack:
			// ビームを削除
			if (m_beam)
			{
				Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
					.getPhysicsFactory()->removeBody(m_beam->getID());
				m_beam.reset();
				m_beamListener.reset();
			}

			// 攻撃終了処理
			m_body->setVelocity(Vec2::Zero()); // 速度をリセット
			attackState = AttackState::AttackStart;
			changeAI(AIType::Patrol); // Patrolに戻る
			break;

		default:
			attackState = AttackState::AttackStart;
			attackTimer = 0.0;
			swingTimer = 0.0;
			break;
		}
	}

	void Eye::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
		switch (other->getLayer())
		{
		case Jam::Domain::Physics::PhysicsLayer::Player	:
			m_eventQueue.push(Events::PlayerDamagedEvent{
					m_body->getID(),
					m_playerId,
					DamageInfo {
					m_status.attackPower,
					m_body->getPosition(),
					(getPlayerPos() - m_body->getPosition()).normalized(),
					true,
					false
					}
					,0.0
					,0.3
					,15.0
				});
			break;
		default:
			break;
		}
	}

	void Eye::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) {}

	void Eye::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionExit(other);
	}
}
