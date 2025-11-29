#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include "Domain/IIndependentObject.h"

namespace Jam::Infrastructure
{
	// 独立オブジェクト管理クラス
	// 生成者と主従の関係でないオブジェクトを管理する
	class IndependentObjectFactory
	{
	private:
		std::unordered_map<int, std::shared_ptr<Jam::Domain::IIndependentObject>> m_objects;
		std::vector<int> m_pendingRemove; // 削除予定ID
		int m_nextID = 0;

	public:
		static IndependentObjectFactory& instance();

		//オブジェクトの登録・削除
		int registerObject(const std::shared_ptr<Jam::Domain::IIndependentObject>& obj);
		void removeObject(int id);
		void removeObjectByPtr(Jam::Domain::IIndependentObject* obj);

		//削除待ちリストに追加
		void flushPendingRemovals();
		//全オブジェクト削除
		void clearAllObjects();

		auto& getObjects() { return m_objects; }
		const auto& getObjects() const { return m_objects; }

	private:
		IndependentObjectFactory() = default;
	};
}
