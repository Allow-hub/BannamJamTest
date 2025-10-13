#pragma once
#include <memory>

namespace Jam::Domain::Physics
{
	class IPhysicsBody;

	class ICollisionListener
	{
	public:
		virtual ~ICollisionListener() = default;
		virtual void onCollisionEnter(std::shared_ptr<IPhysicsBody> other) = 0;
		virtual void onCollisionStay(std::shared_ptr<IPhysicsBody> other) = 0;
		virtual void onCollisionExit(std::shared_ptr<IPhysicsBody> other) = 0;
	};
}
