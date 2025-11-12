#pragma once
#include "IChokerState.h"
#include "ChokerIdleState.h"
#include "../../../Infrastructure/FactoryServiceLocator.h"
#include "../../../Infrastructure/IPhysicsBodyFactory.h"
#include "../../Events/GameEvents.h"

namespace Jam::Domain::Player
{
	class HookedEnemyState : public IChokerState
	{
	private:
		Texture wireTex;
		const double m_enemyJointShrinkSpeed = 0.75;
		const double m_enemyHitFreezeDuration = 0.1;
		const double m_minJointLength = 30.0;
		const double m_invincibilityThreshold = 50.0; // minLength + 50
		const double m_finishThreshold = 1.0;         // minLength + 1
		const double m_reachThreshold = 160.0;
		const double m_launchPower = 2000.0;

		Vec2 m_enemyImpulseDir = Vec2::Zero();
		bool m_isInEnemyHitFreeze = false;
		double m_enemyHitFreezeTimer = 0.0;
		bool m_hasSetInvincible = false;

		Jam::Util::Task delayReset(ChokerContext& ctx)
		{
			co_await Jam::Util::WaitSeconds(0.5);
			ctx.player.setIsInvincible(false);
		}

		void finishEnemySequence(ChokerContext& ctx)
		{
			ctx.targetEnemy = nullptr;
			ctx.isHooked = false;

			// ジョイントをリリース
			if (ctx.joint.has_value())
			{
				ctx.joint->release();
				ctx.joint.reset();
			}

			// フックの物理状態をリセット
			ctx.body->setVelocity({ 0, 0 });
			ctx.body->setAngularVelocity(0);
			ctx.body->setBodyType(PhysicsType::Dynamic);
			ctx.body->setGravityScale(0);

			delayReset(ctx);
			ctx.skill.transitionTo<ChokerIdleState>();
		}

		void createEnemyJoint(ChokerContext& ctx)
		{
			if (!ctx.targetEnemy)
			{
				Print << U"⚠️ ターゲット敵が存在しない";
				finishEnemySequence(ctx);
				return;
			}

			auto& world = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()->getWorld();

			Vec2 hookPos = ctx.body->getPosition();
			auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()->getBody(ctx.ownerId);

			if (!playerBody)
			{
				finishEnemySequence(ctx);
				return;
			}

			Vec2 playerPos = playerBody->getPosition();
			double dist = (hookPos - playerPos).length();
			dist = std::clamp(dist, 1.0, 9999.0);

			// 既存のジョイントをリリース
			if (ctx.joint.has_value())
			{
				ctx.joint->release();
				ctx.joint.reset();
			}

			// フックを静的にする
			ctx.body->setBodyType(PhysicsType::Static);

			// 新しいジョイントを作成
			ctx.joint = ctx.body->createDistanceJoint(world, playerBody, hookPos, playerPos, dist);

			if (ctx.joint.has_value())
			{
				ctx.joint->setLinearStiffness(15.0, 0.8);
				ctx.joint->setMinLength(0.0);
				ctx.joint->setMaxLength(dist);

				Vec2 diff = hookPos - playerPos;
				if (diff.isZero())
					diff = Vec2{ 0, -1 };
				ctx.lastDir = diff.normalized();

				ctx.isHooked = true;
			}
			else
			{
				Print << U"❌ 敵用ジョイント作成失敗";
				finishEnemySequence(ctx);
			}
		}

	public:
		HookedEnemyState()
			: wireTex(U"Assets/Player/wire.png")
		{
		}

		void enter(ChokerContext& ctx) override
		{
			if (!ctx.targetEnemy)
			{
				ctx.skill.transitionTo<ChokerIdleState>();
				return;
			}

			// ヒットフリーズ開始
			m_isInEnemyHitFreeze = true;
			m_enemyHitFreezeTimer = m_enemyHitFreezeDuration;
			m_hasSetInvincible = false;
			ctx.isFlying = false;

			// インパルス方向を計算
			auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()->getBody(ctx.ownerId);

			if (playerBody)
			{
				Vec2 hookPos = ctx.body->getPosition();
				Vec2 playerPos = playerBody->getPosition();
				Vec2 diff = hookPos - playerPos;

				if (diff.isZero())
					diff = Vec2{ 0, -1 };

				m_enemyImpulseDir = diff.normalized().rotated(-2_deg);
			}

			// 物理状態を停止
			ctx.body->setVelocity({ 0, 0 });
			ctx.body->setAngularVelocity(0);
			ctx.targetEnemy->setVelocity(Vec2::Zero());

			// イベント発行
			ctx.eventQueue.push(Events::PlayerChokerSkillEvent{ 0.9, 0.5 });
		}

		void update(ChokerContext& ctx, double deltaTime) override
		{
			// ヒットフリーズ処理
			if (m_isInEnemyHitFreeze)
			{
				m_enemyHitFreezeTimer -= deltaTime;
				if (m_enemyHitFreezeTimer <= 0.0)
				{
					m_isInEnemyHitFreeze = false;
					createEnemyJoint(ctx);
				}
				return;
			}

			// ターゲットの存在確認
			if (!ctx.targetEnemy)
			{
				finishEnemySequence(ctx);
				return;
			}

			// ジョイントの存在確認
			if (!ctx.joint.has_value() || !ctx.isHooked)
			{
				finishEnemySequence(ctx);
				return;
			}

			// プレイヤーボディの取得
			auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()->getBody(ctx.ownerId);

			if (!playerBody)
			{
				finishEnemySequence(ctx);
				return;
			}

			// ジョイントを縮める
			double currentMax = ctx.joint->getMaxLength();
			double newMax = currentMax * m_enemyJointShrinkSpeed;
			ctx.joint->setMaxLength(std::max(newMax, m_minJointLength));

			// 無敵状態の設定
			if (!m_hasSetInvincible && newMax <= m_minJointLength + m_invincibilityThreshold)
			{
				ctx.player.setIsInvincible(true);
				m_hasSetInvincible = true;
			}

			// 最終フェーズ: 敵に到達してダメージ
			if (newMax <= m_minJointLength + m_finishThreshold)
			{
				Vec2 playerPos = playerBody->getPosition();
				Vec2 enemyPos = ctx.targetEnemy->getPosition();
				double distance = (enemyPos - playerPos).length();

				// 距離が遠すぎる場合は中断
				if (distance > m_reachThreshold)
				{
					finishEnemySequence(ctx);
					return;
				}

				// プレイヤーを後方に飛ばす
				ctx.player.controlCooldown(0.5);
				ctx.eventQueue.push(Events::PlayerAttackedEvent{ 1.2, 0.2, 25.2 });
				playerBody->applyImpulse(ctx.lastDir * m_launchPower);

				// 敵にダメージ
				ctx.eventQueue.push(Events::EnemyDamagedEvent{
					playerBody->getID(),
					ctx.targetEnemy->getID(),
					DamageInfo {
						ctx.stats.power,
						ctx.body->getPosition(),
						m_enemyImpulseDir,
						true,
						false
					}
				});

				finishEnemySequence(ctx);
			}
		}

		void draw(const ChokerContext& ctx) const override
		{
			// プレイヤー位置とフック／敵位置
			auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()->getBody(ctx.ownerId);
			if (!playerBody || !ctx.targetEnemy) return;

			Vec2 playerPos = playerBody->getPosition();
			Vec2 hookPos = ctx.body->getPosition();
			drawWire(playerPos, hookPos, wireTex);

			// フック本体を描く
			ctx.body->drawFrame(3.0, Palette::Violet);
		}

		void exit(ChokerContext& ctx) override
		{
			// 状態終了時のクリーンアップ
			m_isInEnemyHitFreeze = false;
			m_enemyHitFreezeTimer = 0.0;
			m_hasSetInvincible = false;
		}

		void onCollisionEnter(ChokerContext& ctx, std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body) override
		{
		}
	};
}
