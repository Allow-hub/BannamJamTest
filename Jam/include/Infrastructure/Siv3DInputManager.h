#pragma once
#include <Siv3D.hpp>
#include "../Domain/IInputService.h"

namespace Jam::Infrastructure
{
	// Siv3Dの入力をDomainのIInputServiceに適合させるアダプター
	// Siv3dのInputクラスを直接使うのではなく、このクラスを介して使う
	// こうすることで、将来的に入力ライブラリを変更したい場合にも、Domain層やUseCase層に影響を与えずに済む
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
			m_state.dash = KeyLShift.pressed();
			m_state.attack = MouseL.down();
			m_state.down = KeyS.pressed();
			m_state.chokerPush = MouseL.down();
			m_state.chokerReleased = MouseL.up();
			m_state.settting = KeyEscape.down();
		}

		Jam::Domain::InputState GetState() const override
		{
			return m_state;
		}
	};
}
