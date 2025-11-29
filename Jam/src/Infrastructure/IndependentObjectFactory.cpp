#include "Infrastructure/IndependentObjectFactory.h"

namespace Jam::Infrastructure
{
	IndependentObjectFactory& IndependentObjectFactory::instance()
	{
		static IndependentObjectFactory inst;
		return inst;
	}

	int IndependentObjectFactory::registerObject(const std::shared_ptr<Jam::Domain::IIndependentObject>& obj)
	{
		int id = m_nextID++;
		m_objects[id] = obj;
		return id;
	}

	void IndependentObjectFactory::removeObject(int id)
	{
		m_objects.erase(id);
	}

	void IndependentObjectFactory::removeObjectByPtr(Jam::Domain::IIndependentObject* obj)
	{
		for (auto& pair : m_objects)
		{
			if (pair.second.get() == obj)
			{
				m_pendingRemove.push_back(pair.first);
				return;
			}
		}
	}

	void IndependentObjectFactory::flushPendingRemovals()
	{
		for (int id : m_pendingRemove)
		{
			m_objects.erase(id);
		}
		m_pendingRemove.clear();
	}

	void IndependentObjectFactory::clearAllObjects()
	{
		m_objects.clear();
		m_pendingRemove.clear();
		m_nextID = 0;
	}
}
