#pragma once
#include <memory>
#include <stdexcept>
#include "../UseCase/EnemyFactory.h"

namespace Jam::Infrastructure::Locator
{
	class IPhysicsBodyFactory;

	class FactoryServiceLocator
	{
	private:
		// 各Factoryの登録先
		std::shared_ptr<IPhysicsBodyFactory> m_physicsFactory;
		Jam::UseCase::EnemyFactory* m_enemyFactory = nullptr;
		// singleton
		FactoryServiceLocator() = default;

	public:
		static FactoryServiceLocator& instance()
		{
			static FactoryServiceLocator inst;
			return inst;
		}

		// 初期化系
		void registerPhysicsFactory(const std::shared_ptr<IPhysicsBodyFactory>& factory)
		{
			m_physicsFactory = factory;
		}

		void registerEnemyFactory(Jam::UseCase::EnemyFactory* factory)
		{
			m_enemyFactory = factory;
		}

		std::shared_ptr<IPhysicsBodyFactory> getPhysicsFactory() const
		{
			if (!m_physicsFactory)
				throw std::runtime_error("PhysicsFactory not registered!");
			return m_physicsFactory;
		}

		Jam::UseCase::EnemyFactory* getEnemyFactory() const
		{
			if (!m_enemyFactory)
				throw std::runtime_error("EnemyFactory not registered!");
			return m_enemyFactory;
		}		// コピー禁止
		FactoryServiceLocator(const FactoryServiceLocator&) = delete;
		FactoryServiceLocator& operator=(const FactoryServiceLocator&) = delete;
	};
}
