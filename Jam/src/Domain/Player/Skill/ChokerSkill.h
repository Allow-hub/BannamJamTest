#pragma once
#include "IPlayerSkill.h"
#include "../../Physics/ICollisionListener.h"
#include "../../Events/GameEvents.h"
#include "../../../Infrastructure/Siv3DCursorUtil.h"

namespace Jam::Domain::Player
{
	class ChokerSkill :public Jam::Domain::Physics::ICollisionListener, public IPlayerSkill, public std::enable_shared_from_this<ChokerSkill>
	{
	private:
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> m_body;
		std::optional<s3d::P2DistanceJoint> m_joint;
		bool m_isActive = false;
		bool m_isFlying = false;
		bool m_isJointCreated = false;  // 追加: ジョイント作成フラグ
		bool m_isHooked = false;  // フックが刺さっているか
		Vec2 m_velocity = Vec2::Zero();
		const Vec2 createOffset = Vec2{ 50,-30 };
		Jam::Domain::Physics::PhysicsBodyID m_ownerId;

		// クールダウン機能
		double m_cooldownTimer = 0.0;
		const double m_cooldownTime = 0.1;  // クールダウン時間（秒）

		// フック時のパラメータ
		const double m_pullImpulse = 300.0;  // 引き寄せの強さ
		const double m_releaseImpulse = 300.0;  // 引き寄せの強さ
		const double m_hookedMoveSpeed = 3;  // フック中の移動速度倍率
		Vec2 m_lastDir;

		// ジョイントを完全に解放するヘルパー関数
		void releaseJoint()
		{
			if (m_joint.has_value())
			{
				m_joint->release();
				m_joint.reset();
			}
			m_isJointCreated = false;  // フラグもリセット
		}

		// フックを完全にリセットするヘルパー関数
		void resetHook()
		{
			m_body->setVelocity({ 0, 0 });
			m_body->setAngularVelocity(0);
			m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Dynamic);
			m_body->setGravityScale(0);
		}

	public:
		ChokerSkill(Jam::Domain::Events::GameEventQueue& eventQueue, Jam::Domain::Physics::PhysicsBodyID ownerId)
			: IPlayerSkill(PlayerSkillType::Choker, eventQueue)
		{
			m_body = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()
				->createBody(
					Vec2{ -10000, -10000 },
					SizeF{ 20, 20 },
					s3d::P2BodyType::Dynamic,
					Jam::Domain::Physics::PhysicsMaterial{ 0.0, 0.0, 0.1 },
					Jam::Domain::Physics::PhysicsShape::Circle
				);
			m_body->setGravityScale(0);
			m_body->setBullet(true);
			m_ownerId = ownerId;
		}

		void init()
		{
			m_body->setCollisionListener(shared_from_this());
		}

		void use(const s3d::Vec2 position, bool facingRight) override
		{
			// クールダウン中は使用不可
			if (m_cooldownTimer > 0.0)
			{
				return;
			}

			// アクティブ中またはジョイントが残っている場合は使用不可
			if (m_isActive || m_joint.has_value())
			{
				return;
			}

			// 念のため完全クリーンアップ
			releaseJoint();
			resetHook();

			Vec2 target = Jam::Infrastructure::CursorUtil::instance().getCursorPosF();

			// プレイヤーの現在位置からオフセットを計算
			bool isRight = (target.x >= position.x);
			Vec2 offset = isRight ? createOffset : Vec2{ -createOffset.x, createOffset.y };

			// プレイヤーの現在位置を基準にフックを配置
			Vec2 hookStartPos = position + offset;

			// 位置を明示的に設定
			m_body->setPos(hookStartPos);

			// 方向を計算して発射
			Vec2 diff = target - hookStartPos;
			if (diff.isZero()) {
				diff = Vec2{ 1, 0 }; // デフォルト方向（右向き）
			}
			Vec2 dir = diff.normalized();
			const double speed = 5000.0;
			m_body->setVelocity(dir * speed);

			m_isActive = true;
			m_isFlying = true;
		}

		void draw() const override
		{
			// クールダウン表示（オプション）
			if (m_cooldownTimer > 0.0)
			{
				// クールダウン中のビジュアルフィードバック
				// 例: 画面左上にクールダウンバーを表示
				const double progress = 1.0 - (m_cooldownTimer / m_cooldownTime);
				const RectF cooldownBar(10, 10, 200 * progress, 10);
				cooldownBar.draw(ColorF(Palette::Violet, 0.5));
			}
			if (!m_isActive)return;
			if (m_joint.has_value())
				m_joint->draw(Palette::Violet);
			m_body->drawFrame(3.0, Palette::Violet);
		}

		void useReleased(const s3d::Vec2 position, bool facingRight) override
		{
			// 何も起きていない場合は早期リターン
			if (!m_isActive && !m_joint.has_value() && !m_isFlying)
			{
				return;
			}
			// ジョイントを完全に解放
			releaseJoint();

			// フックを完全にリセット
			resetHook();

			// 画面外に移動
			m_body->setPos(Vec2{ -10000, -10000 });

			// 状態をリセット（順序重要）
			m_isFlying = false;
			m_isActive = false;

			// クールダウン開始
			m_cooldownTimer = m_cooldownTime;
			auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
				.getPhysicsFactory()
				->getBody(m_ownerId);

			Vec2 impulseDir = m_lastDir;
			if (!impulseDir.hasNaN() || impulseDir.isZero()) {
				// 空中で何もヒットしていない場合は、プレイヤーの向きに応じて上方向か前方向に補正
				impulseDir = facingRight ? Vec2{ 1, -0.5 }.normalized() : Vec2{ -1, -0.5 }.normalized();
			}
			playerBody->applyImpulse(impulseDir * m_releaseImpulse);
		}

		void update(double deltaTime) override
		{
			// クールダウンタイマーを減少
			if (m_cooldownTimer > 0.0)
			{
				m_cooldownTimer -= deltaTime;
				if (m_cooldownTimer <= 0.0)
				{
					m_cooldownTimer = 0.0;
				}
			}

			if (m_joint.has_value() && m_isJointCreated)
			{
				double currentMax = m_joint->getMaxLength();
				double newMax = currentMax * 0.992; // 約0.8%短縮
				double minLimit = 50.0; // 最短距離（めり込み防止）

				if (newMax > minLimit)
				{
					m_joint->setMaxLength(newMax);
				}
				else
				{
					m_joint->setMaxLength(minLimit);
					m_isHooked = true; // 完全に引き寄せ完了
				}
			}
		}

		bool needUpdate() const override
		{
			// アクティブまたはクールダウン中は更新が必要
			return m_isActive || m_cooldownTimer > 0.0;
		}

		void onCollisionEnter(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override
		{
			using namespace Jam::Domain::Physics;
			if (other->getLayer() == PhysicsLayer::Ground && m_isFlying && !m_isJointCreated)
			{
				m_eventQueue.push(Events::PlayerChokerSkillEvent{
					0.8,
					0.7
				});
				m_body->setVelocity({ 0, 0 });
				m_body->setBodyType(Jam::Domain::Physics::PhysicsType::Static);
				m_isFlying = false;
				auto& world = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
					.getPhysicsFactory()
					->getWorld();

				Vec2 hookPos = m_body->getPosition();
				auto playerBody = Jam::Infrastructure::Locator::FactoryServiceLocator::instance()
					.getPhysicsFactory()
					->getBody(m_ownerId);

				if (!playerBody)
				{
					return;
				}

				Vec2 playerPos = playerBody->getPosition();
				double dist = (hookPos - playerPos).length();
				if (!std::isfinite(dist) || dist < 1.0) {
					dist = 1.0; // 最小距離を確保
				}
				// 既存のジョイントを解放（念のため）
				releaseJoint();

				// 距離ジョイントを作成
				m_joint = m_body->createDistanceJoint(
					world,
					playerBody,
					hookPos,
					playerPos,
					dist
				);

				if (m_joint.has_value())
				{
					m_joint->setLinearStiffness(10.0, 1.0);
					m_joint->setMinLength(0.0);
					m_joint->setMaxLength(dist);
					Vec2 diff = hookPos - playerPos;
					if (diff.isZero()) {
						diff = Vec2{ 0, -1 }; // デフォルトで上方向に引く
					}
					m_lastDir = diff.normalized();
					const double angleOffset = -8_deg;
					m_lastDir = m_lastDir.rotated(angleOffset);

					m_isJointCreated = true;  // フラグを立てる
				}
				else
				{
					Print(U"⚠️ ジョイント作成失敗");
				}

			}
		}

		void onCollisionStay(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override {}
		void onCollisionExit(std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> other) override {}

		~ChokerSkill()
		{
			releaseJoint();
		}

		// クールダウン状態を取得（UI表示などに使用）
		bool isOnCooldown() const
		{
			return m_cooldownTimer > 0.0;
		}

		// クールダウンの進行度を取得（0.0～1.0）
		double getCooldownProgress() const
		{
			if (m_cooldownTime <= 0.0) return 1.0;
			return 1.0 - (m_cooldownTimer / m_cooldownTime);
		}

		// 残りクールダウン時間を取得
		double getRemainingCooldown() const
		{
			return m_cooldownTimer;
		}

		bool isFlying()const { return m_isActive; }
		double getHookedMoveSpeedMultiplier()const { return m_hookedMoveSpeed; }

		void onDeactivate()
		{
			if (m_joint.has_value())
			{
				resetHook();
				releaseJoint();
			}
		}
	};
}
