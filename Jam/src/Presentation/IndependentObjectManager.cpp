#include "Presentation/IndependentObjectManager.h"

namespace Jam::Presentation
{
	void IndependentObjectManager::draw()
	{
		const auto& objects = Jam::Infrastructure::IndependentObjectFactory::instance().getObjects();
		for (const auto& [id, obj] : objects)
		{
			obj->draw();
		}
	}
}
