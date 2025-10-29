#pragma once
#include "../Infrastructure/IndependentObjectFactory.h"

namespace Jam::UseCase
{
	//単体オブジェクトの更新をするクラス
	//状態を持たない純粋な処理クラスだからSingletonにする必要がなくstatic関数でおく
	class IndependentObjectService
	{
	public:
		static void update(double dt)
		{
			auto& factory = Jam::Infrastructure::IndependentObjectFactory::instance();
			auto& objects = factory.getObjects();

			for (auto it = objects.begin(); it != objects.end(); )
			{
				auto& obj = it->second;
				obj->update(dt);

				if (obj->isDead())
					it = objects.erase(it);
				else
					++it;
			}

			factory.flushPendingRemovals();
		}
	};
}
