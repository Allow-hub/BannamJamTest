#pragma once

namespace Jam::Domain
{
	struct InputState
	{
		bool left = false;
		bool right = false;
		bool jump = false;
	};

	class IInputService
	{
	public:
		virtual ~IInputService() = default;
		virtual void Update() = 0;
		virtual InputState GetState() const = 0;
	};
}
