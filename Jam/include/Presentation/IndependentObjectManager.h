#pragma once
#include "../Infrastructure/IndependentObjectFactory.h"

namespace Jam::Presentation
{
	// 単体オブジェクトの描画を担当するクラス
	class IndependentObjectManager
	{
	public:
		static void draw();
	};
}
