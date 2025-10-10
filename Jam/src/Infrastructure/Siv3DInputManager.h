#pragma once
#include <Siv3D.hpp>
#include "../Domain/IInputService.h"

namespace Jam::Infra
{
	class Siv3DInputManager final : public Jam::Domain::IInputService
	{
	private:
		Jam::Domain::InputState m_state;

	public:
		void Update() override
		{
			m_state.left = KeyA.pressed();
			m_state.right = KeyD.pressed();
			m_state.jump = KeySpace.down() || KeyW.down();
		}

		Jam::Domain::InputState GetState() const override
		{
			return m_state;
		}
	};
}
