# include "ProjectileFactory.h"
# include "Bomb.h"
//# include "FireBall.h"

std::unique_ptr<ProjectileBase> ProjectileFactory::create(
	ProjectileType type,
	P2World& world,
	const Vec2& enemyPos,
	const Vec2& playerPos)
{
	switch (type)
	{
	case ProjectileType::Bomb:
		// std::make_unique を使って Bomb を生成し、unique_ptr<ProjectileBase> にアップキャストして返す
		return std::make_unique<Bomb>(world, enemyPos, playerPos);
		break;
	case ProjectileType::FireBall:
		//return std::make_unique<FireBall>(enemyPos, playerPos);
		break;
	}
	// 万が一、未定義のTypeが来たら nullptr を返す
	return nullptr;
}
