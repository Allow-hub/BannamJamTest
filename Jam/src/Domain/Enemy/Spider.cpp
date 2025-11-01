#include "Spider.h"
#include "EnemyAI/PatrolAI.h"
#include "EnemyAI/ChaseAI.h"
#include "EnemyAI/AttackAI.h"

// Siv3D.hpp (Vec2, Print のため)
#include <Siv3D.hpp> 
// IPhysicsBody.h (applyImpulse, getVelocity, setVelocity のため)
#include "../Physics/IPhysicsBody.h"

namespace Jam::Domain::Enemy
{

	Spider::Spider(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body, Jam::Domain::Physics::PhysicsBodyID playerId
	, Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
	{
		// AIのリストを作成
		std::vector<std::pair<AIType, std::unique_ptr<IEnemyAI>>> aiList;
		aiList.emplace_back(AIType::Attack, std::make_unique<AttackAI>());
		aiList.emplace_back(AIType::Patrol, std::make_unique<PatrolAI>());
		aiList.emplace_back(AIType::Chase, std::make_unique<ChaseAI>());

		// AIリストを基底クラスにセット (PatrolAI の onPatrolEnter が呼ばれる)
		setAIList(std::move(aiList));

		m_enemyType = EnemyType::Spider;
		m_body->setGravityScale(1); // スパイダーの重力を設定
	}


	void Spider::update(double deltaTime)
	{
		if (!isAlive()) return;

		// クールダウンタイマーを進める
		// (Spider.h で m_attackCooldownTimer が定義されている前提)
		if (m_attackCooldownTimer > 0.0)
		{
			m_attackCooldownTimer -= deltaTime;
		}

		AIType currentType = getAIType();
		const double distance = getPlayerPos().distanceFrom(getPosition());

		m_currentAI->update(*this, deltaTime);
	}


	void Spider::onAttackEnter()
	{
		if (m_attackCooldownTimer > 0.0)
		{
			//Print << U"Spider: Attack canceled (Cooldown)";
			// 攻撃がキャンセルされたフラグを立てる
			// (m_attackCanceled は .h で定義されている前提)
			m_attackCanceled = true;
			// ジャンプ処理を実行しない
			return;
		}


		//Print << U"Spider: Pounce!";

		const Vec2 playerPos = getPlayerPos();
		const Vec2 myPos = getPosition();
		const double directionX = (playerPos.x > myPos.x) ? 1.0 : -1.0;

		if(directionX < 0)
			m_isFaceLeft = true;
		else
			m_isFaceLeft = false;

		const Vec2 pounceImpulse = {
			directionX * pounceHorizonalImpulse,
			-pounceVerticalImpulse
		};

		m_body->applyImpulse(pounceImpulse);
		m_isGrounded = false;
		m_isLanding = false;
	}


	void Spider::onAttackUpdate(double deltaTime)
	{
		// 1. onAttackEnter で攻撃がキャンセルされた場合
		if (m_attackCanceled)
		{
			if (m_attackCooldownTimer <= 0.0)
			{
				//Print << U"Spider: Cooldown (canceled) complete. Returning to patrol.";
				m_attackCanceled = false;

				// 念のため、Patrolに戻る前に重力を通常に戻す
				m_body->setGravityScale(1.0);
				changeAI(AIType::Patrol);
			}
			// クールダウンが終わるまで、着地ロジックは実行しない
			return;
		}

		// 2. まだ飛んでいて、着地した「瞬間」か？
		// (m_isLanding フラグがまだ立っておらず、m_isGrounded が true になった瞬間に一度だけ実行)
		if (!m_isLanding && m_isGrounded)
		{
			//Print << U"Spider: Landed. Starting pin timer.";

			// 「着地して待機中」フラグを立てる
			// (この後の if (m_isLanding) ブロックが即座に実行される)
			m_isLanding = true;
			// タイマーをリセット
			m_landedTimer = 0.0;
		}

		// 3. 「着地して待機中」なら (上のブロックで true になった直後も含む)
		if (m_isLanding)
		{
			// 物理的な反発 (バウンド) に対抗するため、
			// 待機中は「重力を0」にし、かつ「Y速度を0」に毎フレーム強制する

			// 重力を0にして、落下/重力の影響をなくす
			m_body->setGravityScale(0.0);

			// Y速度を強制的に0にし、バウンドの反発力を相殺する
			Vec2 currentVel = m_body->getVelocity();
			m_body->setVelocity({ currentVel.x, 0.0 });


			// タイマーを進める
			m_landedTimer += deltaTime;

			// 4. 待機時間 (loadingWaitTime) を超えたか？
			if (m_landedTimer >= loadingWaitTime)
			{
				//Print << U"Spider: Wait complete. Returning to patrol.";

				m_isLanding = false;
				m_attackCooldownTimer = ATTACK_COOLDOWN_TIME;

				// --- 💡 修正点 ---
				// AIを変更する「前」に、重力を通常(1.0)に戻す
				m_body->setGravityScale(1.0);
				// --- 修正ここまで ---

				// Patrol 状態に戻す
				changeAI(AIType::Patrol);
			}
		}
	}

	void Spider::onAttackExit()
	{
	}

	void Spider::onAIEvent(EnemyAIEvent e)
	{
		switch (e)
		{
		case EnemyAIEvent::PlayerFound:
			changeAI(AIType::Chase);
			break;

		case EnemyAIEvent::PlayerLost:
			changeAI(AIType::Patrol);
			break;
		case EnemyAIEvent::ReachedGoal:
			changeAI(AIType::Attack);
			break;

		default:
			break;
		}
	}

	void Spider::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);
		switch (other->getLayer())
		{
		case Physics::PhysicsLayer::Player:
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
			m_body->setVelocity({ 0,0 });
			break;
		default:
			break;
		}
	}

	void Spider::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionStay(other);
	}

	void Spider::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionExit(other);
	}
}
