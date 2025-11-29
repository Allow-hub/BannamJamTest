#include "UseCase/IndependentObjectService.h"

namespace Jam::UseCase
{
	void IndependentObjectService::update(double dt)
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
}
