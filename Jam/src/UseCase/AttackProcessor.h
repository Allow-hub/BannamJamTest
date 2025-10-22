#pragma once
#include <memory>
#include <unordered_map>
#include "../Domain/ITakeDamageable.h"
#include "../Domain/Physics/IPhysicsBody.h"

namespace Jam::UseCase
{
	using PhysicsBodyID = Jam::Domain::Physics::PhysicsBodyID;

	// 攻撃実行システム
	class AttackProcessor
	{
	private:
		// PhysicsBodyIDからITakeDamageableへのマップ
		std::unordered_map<PhysicsBodyID, std::weak_ptr<Jam::Domain::ITakeDamageable>> m_damageableMap;

		AttackProcessor() = default;

		// コピー・ムーブ禁止
		AttackProcessor(const AttackProcessor&) = delete;
		AttackProcessor& operator=(const AttackProcessor&) = delete;
		AttackProcessor(AttackProcessor&&) = delete;
		AttackProcessor& operator=(AttackProcessor&&) = delete;

	public:
		static AttackProcessor& getInstance()
		{
			static AttackProcessor instance;
			return instance;
		}
		// ダメージ可能なオブジェクトを登録
		void registerDamageable(
			PhysicsBodyID bodyId,
			std::shared_ptr<Jam::Domain::ITakeDamageable> damageable)
		{
			m_damageableMap[bodyId] = damageable;
		}

		// ダメージ可能なオブジェクトを登録解除
		void unregisterDamageable(PhysicsBodyID bodyId)
		{
			m_damageableMap.erase(bodyId);
		}

		// 攻撃実行
		bool executeAttack(
			PhysicsBodyID attackerBodyId,
			PhysicsBodyID targetBodyId,
			const Jam::Domain::DamageInfo& damageInfo)
		{

			// ターゲットがダメージ可能か確認
			auto it = m_damageableMap.find(targetBodyId);
			if (it == m_damageableMap.end())
			{
				return false;
			}

			// weak_ptrから実体を取得
			auto damageable = it->second.lock();
			if (!damageable)
			{
				// オブジェクトが既に破棄されている場合は登録を削除
				m_damageableMap.erase(it);
				return false;
			}

			// ダメージを適用
			damageable->takeDamage(damageInfo);
			return true;
		}

		// 衝突イベントから攻撃を処理
		bool handleCollisionAttack(
			PhysicsBodyID attackerBodyId,
			PhysicsBodyID targetBodyId,
			Vec2 collisionPoint,
			double baseDamage)
		{
			Jam::Domain::DamageInfo info;
			info.amount = baseDamage;
			info.position = collisionPoint;
			info.direction = Vec2::Zero(); // 必要に応じて計算
			info.isCritical = false; // 必要に応じて判定

			return executeAttack(attackerBodyId, targetBodyId, info);
		}

		// 無効なエントリをクリーンアップ
		void cleanup()
		{
			for (auto it = m_damageableMap.begin(); it != m_damageableMap.end();)
			{
				if (it->second.expired())
				{
					it = m_damageableMap.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		void reset()
		{
			m_damageableMap.clear();
		}

		size_t getRegisteredCount() const
		{
			return m_damageableMap.size();
		}
	};
}
