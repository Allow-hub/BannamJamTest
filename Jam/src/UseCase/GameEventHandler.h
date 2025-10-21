#pragma once
#include "../Domain/Events/GameEvents.h"
#include "CameraEvent.h"

namespace Jam::UseCase
{
	// Domainイベントをカメライベントに変換するハンドラー
	class GameEventHandler
	{
	private:
		Domain::Events::GameEventQueue& m_gameEventQueue;
		CameraEventQueue& m_cameraEventQueue;

	public:
		GameEventHandler(Domain::Events::GameEventQueue& gameEventQueue,
						 CameraEventQueue& cameraEventQueue)
			: m_gameEventQueue(gameEventQueue)
			, m_cameraEventQueue(cameraEventQueue)
		{
		}

		void processEvents()
		{
			while (!m_gameEventQueue.empty())
			{
				auto event = m_gameEventQueue.pop();//Queueを削除

				std::visit([this](auto&& e) {
					using T = std::decay_t<decltype(e)>;

					if constexpr (std::is_same_v<T, Domain::Events::EnemyDamagedEvent>)
					{
						handleEnemyDamaged(e);
					}
					else if constexpr (std::is_same_v<T, Domain::Events::EnemyDefeatedEvent>)
					{
						handleEnemyDefeated(e);
					}
					else if constexpr (std::is_same_v<T, Domain::Events::PlayerAttackedEvent>)
					{
						handlePlayerAttacked(e);
					}
					else if constexpr (std::is_same_v<T, Domain::Events::PlayerChokerSkillEvent>)
					{
						handlePlayerChokerSkilled(e);
					}
					else if constexpr (std::is_same_v<T, Domain::Events::PlayerDamagedEvent>)
					{
						handlePlayerDamaged(e);
					}
					else if constexpr (std::is_same_v<T, Domain::Events::BossAppearedEvent>)
					{
						handleBossAppeared(e);
					}
					else if constexpr (std::is_same_v<T, Domain::Events::ItemCollectedEvent>)
					{
						handleItemCollected(e);
					}
				}, event);
			}
		}

	private:
		void handleEnemyDamaged(const Domain::Events::EnemyDamagedEvent& e)
		{
			if (e.isCritical)
			{
				m_cameraEventQueue.push(CameraShakeEvent{ 12.0, 2.3 });
			}
			else
			{
				m_cameraEventQueue.push(CameraShakeEvent{ 5.0, 0.15 });
			}
		}

		void handleEnemyDefeated(const Domain::Events::EnemyDefeatedEvent& e)
		{
			if (e.isBoss)
			{
				// ボス撃破時は派手な演出
				m_cameraEventQueue.push(CameraShakeEvent{ 1.0, 1.0 });
				//m_cameraEventQueue.push(CameraFocusEvent{ e.position, 3.0, 0.9 });
			}
			else
			{
				m_cameraEventQueue.push(CameraShakeEvent{ 8.0, 0.2 });
			}
		}

		void handlePlayerAttacked(const Domain::Events::PlayerAttackedEvent& e)
		{
			m_cameraEventQueue.push(CameraShakeEvent{ e.intensity,e.duration });
			m_cameraEventQueue.push(CameraZoomEvent{e.zoom, e.duration });
		}

		void handlePlayerChokerSkilled(const Domain::Events::PlayerChokerSkillEvent& e)
		{
			m_cameraEventQueue.push(CameraZoomEvent{ e.zoom,e.duration });
		}
		void handlePlayerDamaged(const Domain::Events::PlayerDamagedEvent& e)
		{
			// プレイヤーがダメージを受けた時の演出
			m_cameraEventQueue.push(CameraShakeEvent{ 15.0, 0.3 });
		}

		void handleBossAppeared(const Domain::Events::BossAppearedEvent& e)
		{
			// ボス登場演出
			m_cameraEventQueue.push(CameraFocusEvent{ e.position, 2.5, 0.8 });
		}

		void handleItemCollected(const Domain::Events::ItemCollectedEvent& e)
		{
			if (e.isRare)
			{
				// レアアイテム取得時の演出
				m_cameraEventQueue.push(CameraZoomEvent{ 1.3, 0.5 });
			}
		}
	};
}
