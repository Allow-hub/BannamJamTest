#pragma once
#include <Siv3D.hpp>

namespace Jam
{
	namespace Game
	{
		class InputManager
		{
		public:
			void update()
			{
				m_leftPressed = KeyA.pressed();
				m_rightPressed = KeyD.pressed();
				m_jumpTriggered = KeySpace.down() || KeyW.down();
			}

			// 横移動
			bool left() const { return m_leftPressed; }
			bool right() const { return m_rightPressed; }
			// ジャンプ
			bool jump() const { return m_jumpTriggered; }

		private:
			bool m_leftPressed = false;
			bool m_rightPressed = false;
			bool m_jumpTriggered = false;
		};

	}
}
