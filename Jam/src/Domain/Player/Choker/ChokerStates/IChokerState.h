#pragma once
#include <memory>
#include "../../../Physics/IPhysicsBody.h"
#include "../ChokerContext.h"

namespace Jam::Domain::Player
{
	class ChokerSkill;
	struct ChokerContext;

	class IChokerState
	{
	public:
		virtual ~IChokerState() = default;
		virtual void enter(ChokerContext& ctx) {}
		virtual void update(ChokerContext& ctx, double deltaTime) = 0;
		virtual void exit(ChokerContext& ctx) {}

		virtual void draw(const ChokerContext& ctx) const {}
		virtual void onCollisionEnter(ChokerContext& ctx, std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> body) {}
	};
}
