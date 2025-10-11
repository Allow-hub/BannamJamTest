#pragma once
#include <Siv3D.hpp>
namespace Jam
{
	namespace Objects
	{
		class GameObject
		{
		protected:
			Vec2 position;
			bool active = true;
			bool visible = true;

		public:
			virtual ~GameObject() = default;

			virtual void update(double deltaTime) {}
			virtual void draw() const {}

			Vec2 getPos() const { return position; }
			void setPos(const Vec2& pos) { position = pos; }

			bool isActive() const { return active; }
			void setActive(bool v) { active = v; }

			bool isVisible() const { return visible; }
			void setVisible(bool v) { visible = v; }
		};

	}
}
