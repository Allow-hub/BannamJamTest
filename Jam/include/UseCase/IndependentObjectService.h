#pragma once
#include "Infrastructure/IndependentObjectFactory.h"

namespace Jam::UseCase
{
	// 単体オブジェクトの更新をするクラス
	// 状態を持たない純粋な処理クラスだから Singleton にする必要がなく static 関数でおく
	class IndependentObjectService
	{
	public:
		static void update(double dt);
	};
}
