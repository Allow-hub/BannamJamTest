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
		bool skill = false;//チョーカーや爆弾
		double skillChange = false;//ホイールでスキル変更
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
