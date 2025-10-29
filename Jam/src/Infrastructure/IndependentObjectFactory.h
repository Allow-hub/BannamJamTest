#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include "../Domain/IIndependentObject.h"

namespace Jam::Infrastructure
{
	class IndependentObjectFactory
	{
	private:
		std::unordered_map<int, std::shared_ptr<Jam::Domain::IIndependentObject>> m_objects;
		std::vector<int> m_pendingRemove; // 削除予定ID
		int m_nextID = 0;

	public:
		static IndependentObjectFactory& instance()
		{
			static IndependentObjectFactory inst;
			return inst;
		}

		// 登録
		int registerObject(const std::shared_ptr<Jam::Domain::IIndependentObject>& obj)
		{
			int id = m_nextID++;
			m_objects[id] = obj;
			return id;
		}

		// 通常の即時削除
		void removeObject(int id)
		{
			m_objects.erase(id);
		}

		// ポインタ指定で削除予定に登録
		void removeObjectByPtr(Jam::Domain::IIndependentObject* obj)
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

		// updateの最後に呼んで削除
		void flushPendingRemovals()
		{
			for (int id : m_pendingRemove)
			{
				m_objects.erase(id);
			}
			m_pendingRemove.clear();
		}

		void clearAllObjects()
		{
			m_objects.clear();
			m_pendingRemove.clear();
			m_nextID = 0;
		}

		auto& getObjects() { return m_objects; }
		const auto& getObjects() const { return m_objects; }

	private:
		IndependentObjectFactory() = default;
	};
}
