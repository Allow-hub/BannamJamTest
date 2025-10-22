#pragma once
#include "IPhysicsBodyFactory.h"
#include "Siv3DPhysicsBody.h"
#include <unordered_map>

namespace Jam::Infrastructure::Locator
{
	class Siv3DPhysicsBodyFactory : public IPhysicsBodyFactory
	{
	private:
		P2World* m_world = nullptr;

		// PhysicsBodyID をキーにする
		std::unordered_map<Jam::Domain::Physics::PhysicsBodyID, std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>> m_bodies;

		std::function<void(std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>)> m_onBodyCreated;

	public:
		Siv3DPhysicsBodyFactory() = default;

		void initialize(P2World& world)
		{
			m_world = &world;
		}

		void setBodyCreatedCallback(
			std::function<void(std::shared_ptr<Jam::Infrastructure::Physics::Siv3DPhysicsBody>)> callback)
		{
			m_onBodyCreated = callback;
		}

		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> createBody(
			const Vec2& pos,
			const SizeF& size = SizeF{ 40, 80 },
			s3d::P2BodyType bodyType = s3d::P2BodyType::Dynamic,
			const Jam::Domain::Physics::PhysicsMaterial& material = { 0.2, 0.0, 1.0 },
			const Jam::Domain::Physics::PhysicsShape shape = Jam::Domain::Physics::PhysicsShape::Rect
		) override
		{
			if (!m_world)
				throw std::runtime_error("Siv3DPhysicsBodyFactory not initialized!");

			auto body = std::make_shared<Jam::Infrastructure::Physics::Siv3DPhysicsBody>(
				*m_world, pos, size, bodyType, material, shape
			);

			// P2BodyID を PhysicsBodyID に変換して登録
			Jam::Domain::Physics::PhysicsBodyID id = static_cast<Jam::Domain::Physics::PhysicsBodyID>(body->getBody().id());
			m_bodies.emplace(id, body);

			// コールバック
			if (m_onBodyCreated)
			{
				m_onBodyCreated(body);
			}

			return body;
		}

		// PhysicsBodyID からボディを取得
		[[nodiscard]]
		std::shared_ptr<Jam::Domain::Physics::IPhysicsBody> getBody(Jam::Domain::Physics::PhysicsBodyID id) const override
		{
			if (auto it = m_bodies.find(id); it != m_bodies.end())
			{
				return it->second;
			}
			return nullptr;
		}

		// ボディが存在するか確認
		[[nodiscard]]
		bool hasBody(Jam::Domain::Physics::PhysicsBodyID id) const noexcept
		{
			return m_bodies.contains(id);
		}

		// ボディを削除（物理空間からも消す場合）
		void removeBody(Jam::Domain::Physics::PhysicsBodyID id) override
		{
			if (auto it = m_bodies.find(id); it != m_bodies.end())
			{
				it->second->getBody().release(); // 物理的に削除
				m_bodies.erase(it);
			}
		}

		[[nodiscard]]
		P2World& getWorld() const override
		{
			if (!m_world)
			{
				throw std::runtime_error("Siv3DPhysicsBodyFactory::getWorld() - World is not initialized!");
			}
			return *m_world;
		}
	};
}
