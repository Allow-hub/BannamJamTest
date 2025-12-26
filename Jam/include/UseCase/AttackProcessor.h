#pragma once
#include <memory>
#include <unordered_map>
#include "Domain/ITakeDamageable.h"
#include "Domain/Physics/IPhysicsBody.h"

namespace Jam::UseCase
{
	using PhysicsBodyID = Jam::Domain::Physics::PhysicsBodyID;

	// 攻撃実行クラス
	class AttackProcessor
	{
	private:
		std::unordered_map<PhysicsBodyID, std::weak_ptr<Jam::Domain::ITakeDamageable>> m_damageableMap;

		AttackProcessor();

		// コピー・ムーブ禁止
		AttackProcessor(const AttackProcessor&) = delete;
		AttackProcessor& operator=(const AttackProcessor&) = delete;
		AttackProcessor(AttackProcessor&&) = delete;
		AttackProcessor& operator=(AttackProcessor&&) = delete;

	public:
		static AttackProcessor& getInstance();

		//攻撃可能なBodyを登録
		void registerDamageable(PhysicsBodyID bodyId, std::shared_ptr<Jam::Domain::ITakeDamageable> damageable);

		//攻撃可能なBodyを登録解除
		void unregisterDamageable(PhysicsBodyID bodyId);

		//攻撃を実行
		bool executeAttack(PhysicsBodyID attackerBodyId,
						   PhysicsBodyID targetBodyId,
						   const Jam::Domain::DamageInfo& damageInfo);

		// 登録済みの Damageable を取得（状態異常付与など非ダメージ用途）
		std::shared_ptr<Jam::Domain::ITakeDamageable> getDamageable(PhysicsBodyID bodyId)
		{
			auto it = m_damageableMap.find(bodyId);
			if (it == m_damageableMap.end())
				return nullptr;
			return it->second.lock();
		}

		void cleanup();
		void reset();
		size_t getRegisteredCount() const;
	};
}
