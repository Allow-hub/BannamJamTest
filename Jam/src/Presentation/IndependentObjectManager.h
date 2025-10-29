#pragma once
#include "../Infrastructure/IndependentObjectFactory.h"

namespace Jam::Presentation
{
	//単体オブジェクトの描画担当するクラス
	class IndependentObjectManager
	{
	public:
		static void draw()
		{
			const auto& objects = Jam::Infrastructure::IndependentObjectFactory::instance().getObjects();
			for (const auto& [id, obj] : objects)
			{
				obj->draw();
			}
		}
	};
}
