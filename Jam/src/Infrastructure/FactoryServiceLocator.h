#pragma once
#include <memory>
#include <stdexcept>

namespace Jam::Infrastructure::Locator
{
	class IPhysicsBodyFactory;

	class FactoryServiceLocator
	{
	private:
		// 各Factoryの登録先
		std::shared_ptr<IPhysicsBodyFactory> m_physicsFactory;

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

		std::shared_ptr<IPhysicsBodyFactory> getPhysicsFactory() const
		{
			if (!m_physicsFactory)
				throw std::runtime_error("PhysicsFactory not registered!");
			return m_physicsFactory;
		}

		// コピー禁止
		FactoryServiceLocator(const FactoryServiceLocator&) = delete;
		FactoryServiceLocator& operator=(const FactoryServiceLocator&) = delete;
	};
}
