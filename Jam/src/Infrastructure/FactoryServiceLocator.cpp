#include "Infrastructure/FactoryServiceLocator.h"
#include "Infrastructure/IPhysicsBodyFactory.h"

namespace Jam::Infrastructure::Locator
{
	FactoryServiceLocator& FactoryServiceLocator::instance()
	{
		static FactoryServiceLocator inst;
		return inst;
	}

	void FactoryServiceLocator::registerPhysicsFactory(const std::shared_ptr<IPhysicsBodyFactory>& factory)
	{
		m_physicsFactory = factory;
	}

	void FactoryServiceLocator::registerEnemyFactory(Jam::UseCase::EnemyFactory* factory)
	{
		m_enemyFactory = factory;
	}

	std::shared_ptr<IPhysicsBodyFactory> FactoryServiceLocator::getPhysicsFactory() const
	{
		if (!m_physicsFactory)
			throw std::runtime_error("PhysicsFactory not registered!");
		return m_physicsFactory;
	}

	Jam::UseCase::EnemyFactory* FactoryServiceLocator::getEnemyFactory() const
	{
		if (!m_enemyFactory)
			throw std::runtime_error("EnemyFactory not registered!");
		return m_enemyFactory;
	}
}
