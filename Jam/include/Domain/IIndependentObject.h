#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain
{
	//特定の生成者に従属しない単体オブジェクト
	class IIndependentObject
	{
	protected:
		bool m_isDead = false;

	public:
		virtual ~IIndependentObject() = default;

		virtual void update(double deltaTime) = 0;
		virtual void draw() const = 0;

		bool isDead() const noexcept { return m_isDead; }
		void markDead() noexcept { m_isDead = true; }
	};
}
