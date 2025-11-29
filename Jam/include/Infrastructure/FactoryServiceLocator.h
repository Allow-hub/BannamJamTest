#pragma once
#include <memory>
#include <stdexcept>
#include "UseCase/EnemyFactory.h"

namespace Jam::Infrastructure::Locator
{
	class IPhysicsBodyFactory;

	// Factoryの依存関係を一元管理するロケータクラス
	class FactoryServiceLocator
	{
	private:
		std::shared_ptr<IPhysicsBodyFactory> m_physicsFactory;
		Jam::UseCase::EnemyFactory* m_enemyFactory = nullptr;

		FactoryServiceLocator() = default;

	public:
		static FactoryServiceLocator& instance();

		//ロケータにFactoryを登録
		void registerPhysicsFactory(const std::shared_ptr<IPhysicsBodyFactory>& factory);
		void registerEnemyFactory(Jam::UseCase::EnemyFactory* factory);

		std::shared_ptr<IPhysicsBodyFactory> getPhysicsFactory() const;
		Jam::UseCase::EnemyFactory* getEnemyFactory() const;

		// コピー禁止
		FactoryServiceLocator(const FactoryServiceLocator&) = delete;
		FactoryServiceLocator& operator=(const FactoryServiceLocator&) = delete;
	};
}
