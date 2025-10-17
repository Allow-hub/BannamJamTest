#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain::Player
{
	enum class PlayerSkillType
	{
		Bomb,
		Choker,
	};

	//プレイヤースキルが共通して持つべきメソッド群
	class IPlayerSkill
	{
	protected:
		PlayerSkillType m_type;

	public:
		explicit IPlayerSkill(PlayerSkillType type)
			: m_type(type) {
		}

		virtual ~IPlayerSkill() = default;

		/// スキル実行
		virtual void use(const s3d::Vec2 position, bool facingRight) = 0;
		virtual void useReleased(const s3d::Vec2 position, bool facingRight) = 0;

		/// 毎フレーム更新（必要なら）
		virtual void update(double deltaTime) = 0;

		/// 更新が必要かどうか（スリープ中ならfalse）
		virtual bool needUpdate() const = 0;

		/// スキル種別を取得
		PlayerSkillType getType() const noexcept { return m_type; }

		virtual void draw() const = 0;
	};
}
