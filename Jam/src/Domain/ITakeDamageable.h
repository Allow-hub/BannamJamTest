#pragma once

namespace Jam::Domain
{
	struct DamageInfo
	{
		double amount = 0.0;
		Vec2 position =  {0,0};
		Vec2 direction = {0,0};
		bool isCritical = false;
	};

	//ダメージを受けることが可能なオブジェクトにつける
	class ITakeDamageable
	{
	public:
		virtual ~ITakeDamageable() = default;
		virtual void takeDamage(const DamageInfo& info) = 0;
		virtual bool isAlive() const = 0;
		virtual double getCurrentHp() const = 0;
	};
}
