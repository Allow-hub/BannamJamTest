#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Player
{
	struct IPlayerSkill
	{
		virtual ~IPlayerSkill() = default;

		/// スキル実行
		virtual void use(s3d::Vec2 position, bool facingRight) = 0;

		/// 毎フレーム更新（必要なら）
		virtual void update(double deltaTime) = 0;

		virtual bool needUpdate() = 0;
	};
}
