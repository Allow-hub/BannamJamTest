#include "Boss1_3.h"
#include "Missile.h"
#include "../../Infrastructure/FactoryServiceLocator.h"
#include "../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../Infrastructure/PhysicsFilterManager.h"
#include "../../Infrastructure/IndependentObjectFactory.h"
#include "../../Foundation/CoroutineUtil.h"
#include <random>
#include "Bomb.h"
#include "../../UseCase/AttackProcessor.h"
#include "../../Presentation/AudioService.h"

namespace Jam::Domain::Enemy
{
#pragma region コンストラクタ
	Boss1_3::Boss1_3(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body,
					 Jam::Domain::Physics::PhysicsBodyID playerId,
					 Jam::Domain::Events::GameEventQueue& eventQueue)
		: EnemyBase(body, playerId, eventQueue)
		, currentBossState(BossState::Appear)
		, currentAttackState(AttackState::Missile)
		, m_stateTimer(0.0)
		, m_weakStateDuration(5.0)
		, m_appearDuration(0.1)
		, m_isReflectedMissileHit(false)
		, m_attackCooldownTimer(0.0)
		, m_attackCooldown(1.0)
		, m_isAttacking(false)
		, m_hasAttackEntered(false)
		, m_missileAttackDuration(3.0)
		, m_summonClownDuration(2.0)
		, m_bombAttackDuration(4.5)
		, m_shockWaveDuration(5.0)
		, m_coreOffset(-70, 15)
	{
		m_enemyType = EnemyType::Boss1_3;
		m_body->setGravityScale(2.0);

		// 攻撃パターンの確率設定
		m_attackPatterns = {
			{AttackState::Missile,0.3f},
			{AttackState::SummonClown, 0.1f},
			{AttackState::Bomb, 0.3f},
			{AttackState::Shockwave,0.3f}
		};

		m_body->setFilter(Jam::Infrastructure::PhysicsFilter::BossHidden);//チョーカーとの接触をなくす
		m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Dynamic);
		//弱点ボディ
		m_weakBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
			.getPhysicsFactory()->createBody(m_body->getPosition(), coreSize);
		m_weakBody->setLayer(Jam::Domain::Physics::PhysicsLayer::Enemy);
		m_weakBody->setBodyType(Jam::Domain::Physics::PhysicsType::Static);
		m_weakBody->setFilter(Jam::Infrastructure::PhysicsFilter::Team2Death);
		m_weakBody->setPos(m_body->getPosition() + m_coreOffset);
		m_weakBody->setGravityScale(0);

		m_isFaceLeft = true;

		init();
	}

	Jam::Util::Task Boss1_3::init()
	{
		co_await Jam::Util::WaitSeconds(0.1);
		m_weakBody->setCollisionListener(shared_from_this());
		Jam::UseCase::AttackProcessor::getInstance().registerDamageable(m_weakBody->getID(), shared_from_this());

	}
#pragma endregion

#pragma region 更新・描画
	void Boss1_3::update(double deltaTime)
	{
		if (!isAlive()) return;

		m_stateTimer += deltaTime;
		double dir = m_isFaceLeft ? -1.0 : 1.0;
		m_weakBody->setPos(m_body->getPosition() + m_coreOffset * m_isFaceLeft);
		switch (currentBossState)
		{
		case BossState::Appear:
			updateAppearState(deltaTime);
			break;
		case BossState::Normal:
			updateNormalState(deltaTime);
			break;
		case BossState::Weak:
			updateWeakState(deltaTime);
			break;
		case BossState::Dead:
			// 死亡演出処理
			break;
		}
	}

	void Boss1_3::draw() const
	{
		m_weakBody->drawFrame(5.0, Palette::Blue);

		if (m_shockWave)
		{
			m_shockWave->draw();
		}
	}
#pragma endregion

#pragma region ステート管理
	void Boss1_3::updateAppearState(double deltaTime)
	{
		// 登場演出
		if (m_stateTimer >= m_appearDuration)
		{
			currentBossState = BossState::Normal;
			m_stateTimer = 0.0;
			m_attackCooldownTimer = 0.0;
		}
	}

	void Boss1_3::updateNormalState(double deltaTime)
	{

		if (!m_normalEntered)
		{
			m_body->setFilter(Jam::Infrastructure::PhysicsFilter::BossHidden);
			m_weakBody->setFilter(Jam::Infrastructure::PhysicsFilter::Team2Death);
			m_weakEntered = false;
			m_normalEntered = true;
		}
		// 反射ミサイルが当たったかチェック
		if (m_isReflectedMissileHit)
		{
			// 現在の攻撃を終了
			if (m_isAttacking && m_hasAttackEntered)
			{
				switch (currentAttackState)
				{
				case AttackState::Missile:
					exitMissileAttack();
					break;
				case AttackState::SummonClown:
					exitSummonClown();
					break;
				case AttackState::Bomb:
					exitBombAttack();
					break;
				case AttackState::Shockwave:
					exitShockWave();
					break;
				}
			}

			// Print << U"[Boss1_3] 弱点露出開始";
			currentBossState = BossState::Weak;
			m_stateTimer = 0.0;
			m_isReflectedMissileHit = false;
			m_isAttacking = false;
			m_hasAttackEntered = false;
			m_attackCooldownTimer = 0.0;

			// バリア破壊イベントを発行
			Vec2 bossPos = m_body->getPosition();
			m_eventQueue.push(Events::BarrierShatteredEvent{
				bossPos,
				m_lastMissileDirection,  // ミサイルが当たった方向
				150.0  // バリアの半径
			});

			return;
		}

		// 攻撃中の処理
		if (m_isAttacking)
		{
			m_attackCooldownTimer += deltaTime;

			// 攻撃パターンの実行
			switch (currentAttackState)
			{
			case AttackState::Missile:
				if (!m_hasAttackEntered)
				{
					enterMissileAttack();
					m_hasAttackEntered = true;
				}
				updateMissileAttack(deltaTime);

				if (m_attackCooldownTimer >= m_missileAttackDuration)
				{
					exitMissileAttack();
					m_isAttacking = false;
					m_hasAttackEntered = false;
					m_attackCooldownTimer = 0.0;
				}
				break;

			case AttackState::SummonClown:
				if (!m_hasAttackEntered)
				{
					enterSummonClown();
					m_hasAttackEntered = true;
				}
				updateSummonClown(deltaTime);

				if (m_attackCooldownTimer >= m_summonClownDuration)
				{
					exitSummonClown();
					m_isAttacking = false;
					m_hasAttackEntered = false;
					m_attackCooldownTimer = 0.0;
				}
				break;

			case AttackState::Bomb:
				if (!m_hasAttackEntered)
				{
					enterBombAttack();
					m_hasAttackEntered = true;
				}
				updateBombAttack(deltaTime);

				if (m_attackCooldownTimer >= m_bombAttackDuration)
				{
					exitBombAttack();
					m_isAttacking = false;
					m_hasAttackEntered = false;
					m_attackCooldownTimer = 0.0;
				}
				break;
			case AttackState::Shockwave:
				if (!m_hasAttackEntered)
				{
					enterShockWave();
					m_hasAttackEntered = true;
				}
				updateShockWave(deltaTime);

				if (m_attackCooldownTimer >= m_shockWaveDuration)
				{
					exitShockWave();
					m_isAttacking = false;
					m_hasAttackEntered = false;
					m_attackCooldownTimer = 0.0;
				}
				break;
			}
		}
		else
		{
			// クールダウン中
			m_attackCooldownTimer += deltaTime;

			if (m_attackCooldownTimer >= m_attackCooldown)
			{
				// 次の攻撃を抽選で選択
				currentAttackState = selectNextAttack();
				m_isAttacking = true;
				m_attackCooldownTimer = 0.0;
			}
		}
	}

	void Boss1_3::updateWeakState(double deltaTime)
	{
		if (!m_weakEntered)
		{
			m_body->setFilter(Jam::Infrastructure::PhysicsFilter::WallOnly);
			m_weakBody->setFilter(Jam::Infrastructure::PhysicsFilter::Team2);
			m_normalEntered = false;
			m_weakEntered = true;
		}
		// 弱点露出中は何もしない
		if (m_stateTimer >= m_weakStateDuration)
		{
			// Print << U"[Boss1_3] 弱点露出が終了";
			currentBossState = BossState::Normal;
			m_stateTimer = 0.0;
			m_attackCooldownTimer = 0.0;
			m_isAttacking = false;
			m_hasAttackEntered = false;
		}
	}

	Boss1_3::AttackState Boss1_3::selectNextAttack()
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);

		float roll = dis(gen);
		float cumulative = 0.0f;

		for (const auto& pattern : m_attackPatterns)
		{
			cumulative += pattern.probability;
			if (roll <= cumulative)
			{
				return pattern.state;
			}
		}

		return Boss1_3::AttackState::Missile;
	}
#pragma endregion

#pragma region ミサイル攻撃
	void Boss1_3::enterMissileAttack()
	{
		m_missileAttackTask();
	}
	
	Jam::Util::Task Boss1_3::m_missileAttackTask()
	{
		for (int i = 0; i < MISSILE_COUNT; ++i)
		{
			// プレイヤー位置を取得
			auto physicsFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory();
			auto playerBody = physicsFactory->getBody(m_playerId);
			if (!playerBody) co_return;
			
			Vec2 bossPos = m_body->getPosition();
			Vec2 playerPos = playerBody->getPosition();
			
			// 待機位置を計算(ボスの前方、少し上)
			double waitY = bossPos.y - m_status.colSize.y / 2.0 + 50;
			const double spacing = 120.0;
			Vec2 waitPos = Vec2{ bossPos.x + (i - 1) * spacing, waitY };
			
			// ベジェ曲線の制御点を設定
			Vec2 p0 = waitPos;
			Vec2 p1 = waitPos + Vec2{ 0, -400 };
			Vec2 p2 = playerPos + Vec2{ 0, -400 };
			Vec2 p3 = playerPos;
			
			// 物理ボディをセンサーとして作成（地面を貫通するように）
			auto missileBody = physicsFactory->createCircleSensor(
				waitPos,
				50.0  // 半径
			);
			
		// Missileオブジェクトを作成
		auto missile = std::make_shared<Missile>(
			missileBody,
			m_playerId,
			m_body->getID(),
			m_eventQueue,
			p0, p1, p2, p3,
			m_status.attackPower,
			m_missileFlightDuration,
			m_missileRadius,
			m_missileReflectedSpeed,
			m_missileScale
		);
		missile->init();
		
		// Factoryに登録
		Jam::Infrastructure::IndependentObjectFactory::instance().registerObject(missile);
		
		// 次のミサイルまで待機
		co_await Jam::Util::WaitSeconds(m_missileSpawnInterval);
		}
	}

	void Boss1_3::updateMissileAttack(double deltaTime){}

	void Boss1_3::exitMissileAttack(){}
#pragma endregion

#pragma region ピエロ召喚
	void Boss1_3::enterSummonClown()
	{
		// ServiceLocator経由でFactoryを取得
		auto physicsFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory();
		auto enemyFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getEnemyFactory();

		// Clownのステータスを取得
		const auto& statusTable = enemyFactory->getStatusTable();
		auto it = statusTable.find(Jam::Domain::EnemyType::Clown);

		const auto& clownStatus = it->second;

		// Bossが向いている方向(プレイヤーの方向)を取得
		Vec2 playerPos = getPlayerPos();
		bool isPlayerOnRight = playerPos.x > m_body->getPosition().x;

		// プレイヤーと同じ側(ボスが向いている方向)に生成
		double direction = isPlayerOnRight ? 1.0 : -1.0;

		// 複数体のピエロを生成
		for (int i = 0; i < CLOWN_SPAWN_COUNT; ++i)
		{
			// X座標: Bossの端から
			double bossEdgeX = m_body->getPosition().x + direction * (m_status.colSize.x / 2.0);
			double offsetX = (clownStatus.colSize.x + 10.0) * i;
			double clownCenterX = bossEdgeX + direction * (clownStatus.colSize.x / 2.0 + offsetX);

			// Y座標: Bossの底面 
			double bossBottomY = m_body->getPosition().y + (m_status.colSize.y / 2.0);
			double clownCenterY = bossBottomY - (clownStatus.colSize.y / 2.0);

			Vec2 spawnPos = Vec2(clownCenterX, clownCenterY);

			// 物理ボディを作成
			auto clownBody = physicsFactory->createBody(
				spawnPos,
				clownStatus.colSize,
				s3d::P2BodyType::Dynamic,
				clownStatus.physicsMaterial
			);


			clownBody->setLayer(Jam::Domain::Physics::PhysicsLayer::Enemy);

			// Clown敵を生成
			auto clownEnemy = enemyFactory->createEnemy(
				Jam::Domain::EnemyType::Clown,
				clownBody,
				m_playerId,
				m_eventQueue
			);

			if (clownEnemy)
			{
				clownBody->setCollisionListener(clownEnemy);

				// EnemySpawnedイベントを発行
				m_eventQueue.push(Events::EnemySpawnedEvent{
					clownEnemy,
					U"Assets/Enemy/Clown/Clown_animation.json"  // 今後別の敵のアニメーションのパスを指定する場合は、EnemyTypeから適切なパスを返すヘルパー関数を作成すること
				});
			}
			else
			{
				Console << U"[Boss1_3] ❌ Clown敵の生成失敗[" << i << U"]";
			}
		}
	}

	void Boss1_3::updateSummonClown(double deltaTime)
	{
	}

	void Boss1_3::exitSummonClown()
	{
	}
#pragma endregion

#pragma region 爆弾攻撃
	void Boss1_3::enterBombAttack()
	{
		m_bombAttackTask();
	}

	Jam::Util::Task Boss1_3::m_bombAttackTask()
	{
		for (unsigned int i = 0; i < m_bombsThrown; ++i)
		{
			auto dir = m_isFaceLeft ? -1.0 : 1.0;
			auto offset = Vec2((m_xOffset * i * dir)+dir* m_xBombInitOffset, m_bombSpawnY);
			auto pos = m_body->getPosition() + offset;
			Vec2 size = { 140,140 };
			// 物理ボディを設定
			auto physicsFactory = Jam::Infrastructure::Locator::FactoryServiceLocator::instance().getPhysicsFactory();
			//P2BodyTypeの指定は後で修正
			auto bombBody = physicsFactory->createBody(pos, size,
				s3d::P2BodyType::Dynamic, { 0.2, 0.0, 1.0 }, Jam::Domain::Physics::PhysicsShape::Circle);
			auto bomb = std::make_shared<Jam::Domain::Enemy::Bomb>(bombBody, m_playerId, m_eventQueue, m_status.attackPower, size.x * 1.5, m_explosionDelay, size);
			bomb->init();

			// Factory に登録
			Jam::Infrastructure::IndependentObjectFactory::instance().registerObject(bomb);
			co_await Jam::Util::WaitSeconds(m_bombSpawnInterval);
		}
	}

	void Boss1_3::updateBombAttack(double deltaTime)
	{
	}

	void Boss1_3::exitBombAttack()
	{
	}
#pragma endregion

#pragma region 衝撃波攻撃
	void Boss1_3::enterShockWave()
	{
		// 現在の位置を取得
		Vec2 currentPos = m_body->getPosition();

		// 移動先を決定
		Vec2 targetPos;
		if ((currentPos - m_leftPos).length() < (currentPos - m_rightPos).length())
		{
			targetPos = m_rightPos;
		}
		else
		{
			targetPos = m_leftPos;
		}

		// 放物線ジャンプ処理（Y上方向に強めの力 + X方向に向けた力）
		Vec2 jumpDir = (targetPos - currentPos).normalized();
		double horizontalForce = jumpDir.x * m_shockJumpForce * 1.1; // 横方向
		double verticalForce = -m_shockJumpForce * 1.6;              // 上方向

		m_body->applyImpulse(Vec2(horizontalForce, verticalForce));

		// 衝撃波タスクを開始（ジャンプ後、一定時間経過で地面にShockWave発生）
		m_shockWaveTask(targetPos);
	}


	Jam::Util::Task Boss1_3::m_shockWaveTask(Vec2 targetPos)
	{
		// ジャンプ中の待機
		co_await Jam::Util::WaitSeconds(m_shockWaveDelay);

		// --- 着地処理 ---
		// 速度をゼロにして位置を固定
		m_body->setVelocity(Vec2{ 0, 0 });
		m_body->setPos(targetPos);

		// 向きを反転
		m_isFaceLeft = !m_isFaceLeft;

		// 衝撃波の中心は、ジャンプ先（targetPos）の足元
		Vec2 shockPos = Vec2{ targetPos.x, targetPos.y + (m_status.colSize.y / 2.0) };

		m_shockWave = std::make_shared<ShockWave>(
			shockPos,
			m_shockWaveDuration,
			Vec2{ 100, 150 },
			Vec2{ 4000, 150 },
			m_playerId,
			m_eventQueue
		);
	}



	void Boss1_3::updateShockWave(double deltaTime)
	{
		if (m_shockWave)
		{
			m_shockWave->update(deltaTime);

			// ShockWaveが終了したら破棄
			if (m_shockWave->isFinished())
			{
				m_shockWave.reset();
			}
		}
	}

	void Boss1_3::exitShockWave()
	{
		m_shockWave.reset();
	}

	void Boss1_3::onDestroy(const DamageInfo& info)
	{
		// ボス撃破時の音を再生
		Jam::Presentation::AudioService::get().playOneShot(
			Jam::Presentation::AudioService::Sound::SE_BossDown,
			1.0
		);

		m_eventQueue.push(Events::EnemyDefeatedEvent{
				m_body->getPosition(),
				true,
				m_enemyType,
				0.88,
				0.5,
				10
		});
		EnemyBase::onDestroy(info);
	}
#pragma endregion

#pragma region 衝突処理
	void Boss1_3::onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionEnter(other);

		switch (other->getLayer())
		{
		case Jam::Domain::Physics::PhysicsLayer::Player:
			if (currentBossState == BossState::Weak) {
				currentBossState = BossState::Normal;
			}
			// プレイヤーにダメージを与える
			if (currentBossState != BossState::Normal)return;//Normal以外はダメージ判定を出さない
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
		case Jam::Domain::Physics::PhysicsLayer::Weapon:
			// 反射されたミサイルがボスに当たった
			if (currentBossState == BossState::Normal)
			{
				m_isReflectedMissileHit = true;
				// ミサイルの衝突方向を記録（ボスから見た相対方向）
				m_lastMissileDirection = (other->getPosition() - m_body->getPosition()).normalized();
			}
			break;
		}
	}

	void Boss1_3::onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
	}

	void Boss1_3::onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other)
	{
		EnemyBase::onCollisionExit(other);
	}
#pragma endregion
}
