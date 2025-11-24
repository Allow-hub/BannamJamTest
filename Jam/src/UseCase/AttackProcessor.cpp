#include "AttackProcessor.h"

namespace Jam::UseCase
{
	AttackProcessor::AttackProcessor() = default;

	AttackProcessor& AttackProcessor::getInstance()
	{
		static AttackProcessor instance;
		return instance;
	}

	void AttackProcessor::registerDamageable(PhysicsBodyID bodyId, std::shared_ptr<Jam::Domain::ITakeDamageable> damageable)
	{
		m_damageableMap[bodyId] = damageable;
	}

	void AttackProcessor::unregisterDamageable(PhysicsBodyID bodyId)
	{
		m_damageableMap.erase(bodyId);
	}

	bool AttackProcessor::executeAttack(PhysicsBodyID attackerBodyId,
										PhysicsBodyID targetBodyId,
										const Jam::Domain::DamageInfo& damageInfo)
	{
		auto it = m_damageableMap.find(targetBodyId);
		if (it == m_damageableMap.end())
			return false;

		auto damageable = it->second.lock();
		if (!damageable)
		{
			m_damageableMap.erase(it);
			return false;
		}

		damageable->takeDamage(damageInfo);
		return true;
	}

	void AttackProcessor::cleanup()
	{
		for (auto it = m_damageableMap.begin(); it != m_damageableMap.end(); )
		{
			if (it->second.expired())
				it = m_damageableMap.erase(it);
			else
				++it;
		}
	}

	void AttackProcessor::reset()
	{
		m_damageableMap.clear();
	}

	size_t AttackProcessor::getRegisteredCount() const
	{
		return m_damageableMap.size();
	}
}
