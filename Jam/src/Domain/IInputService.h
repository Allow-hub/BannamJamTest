#pragma once

namespace Jam::Domain
{
	// 入力状態を表す構造体
	struct InputState
	{
		bool left = false;
		bool right = false;
		bool jump = false;
		bool dash = false;
		bool attack = false;
		bool down = false;
		bool chokerPush = false;//チョーカー
		bool chokerReleased = false;
		bool settting = false;//ESCなどで設定画面を表示
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
