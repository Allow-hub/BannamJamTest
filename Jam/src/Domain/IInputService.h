#pragma once

namespace Jam::Domain
{
	// 入力状態を表す構造体
	struct InputState
	{
		bool left = false;
		bool right = false;
		bool jump = false;
	};

	// 入力サービスのインターフェース
	class IInputService
	{
	public:
		virtual ~IInputService() = default;
		virtual void Update() = 0;
		virtual InputState GetState() const = 0;
	};
}
