#pragma once
#include <Siv3D.hpp>
#include "../Domain/Events/GameEvents.h"
#include "CameraEvent.h"
#include "AttackProcessor.h"
#include "EffectEvents.h"
#include "../Foundation/CoroutineUtil.h"
#include "../Presentation/EnemyManager.h"
#include "../Domain/Enemy/Missile.h"
#include "../Infrastructure/IndependentObjectFactory.h"
#include "../Infrastructure/FactoryServiceLocator.h"

namespace Jam::UseCase
{
	// Domainイベントをカメライベントに変換するハンドラー
	class GameEventHandler
	{
	private:
		Domain::Events::GameEventQueue& m_gameEventQueue;
		CameraEventQueue& m_cameraEventQueue;
		EffectEventQueue& m_effectEventQueue;
		std::function<void()> m_onPlayerDeath;
		Jam::Presentation::EnemyManager* m_enemyManager = nullptr;

	public:
		GameEventHandler(
			Domain::Events::GameEventQueue& gameEventQueue,
			CameraEventQueue& cameraEventQueue,
			EffectEventQueue& effectEventQueue,
			std::function<void()> onPlayerDeath,
			Jam::Presentation::EnemyManager* enemyManager)
			: m_gameEventQueue(gameEventQueue)
			, m_cameraEventQueue(cameraEventQueue)
			, m_effectEventQueue(effectEventQueue)
			, m_onPlayerDeath(onPlayerDeath)
			, m_enemyManager(enemyManager)
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
					else if constexpr (std::is_same_v<T, Domain::Events::PlayerDeathEvent>)
					{
						handlePlayerDeath(e);
					}
					else if constexpr (std::is_same_v<T, Domain::Events::PlayerFallOutEvent>)
					{
						handlePlayerFallOut(e);
					}
					else if constexpr (std::is_same_v<T, Domain::Events::BossAppearedEvent>)
					{
						handleBossAppeared(e);
					}
					else if constexpr (std::is_same_v<T, Domain::Events::ItemCollectedEvent>)
					{
						handleItemCollected(e);
					}
					else if constexpr (std::is_same_v<T, Domain::Events::EnemySpawnedEvent>)
					{
						handleEnemySpawned(e);
					}
					else if constexpr (std::is_same_v<T, Domain::Events::ExplosionEvent>)
					{
						handleExplosion(e);
					}
					}, event);
			}
		}

	private:
		void handleEnemyDamaged(const Domain::Events::EnemyDamagedEvent& e)
		{
			Jam::UseCase::AttackProcessor::getInstance().executeAttack(e.attacker, e.target, e.damageInfo);

			constexpr double offsetRange = 2.0;

			// -offsetRange ～ +offsetRange のランダム値をXとYに加える
			Vec2 randomOffset{
				Random(-offsetRange, offsetRange),
				Random(-offsetRange, offsetRange)
			};
			String text = Format(e.damageInfo.amount);
			m_effectEventQueue.push(TextEffectEvent{
				e.damageInfo.position + randomOffset,
				text,
				Palette::Green
			});
			m_effectEventQueue.push(ParticleEffectEvent{
				e.damageInfo.position,
				e.damageInfo.direction,
				Palette::Hotpink,
				100,
				500
			});
		}

		void handleEnemyDefeated(const Domain::Events::EnemyDefeatedEvent& e)
		{
			if (e.isBoss)
			{
				// ボス撃破時は派手な演出
				m_cameraEventQueue.push(CameraShakeEvent{ e.intensity, e.duration });
				//m_cameraEventQueue.push(CameraFocusEvent{ e.position, 3.0, 0.9 });
			}
			else
			{
				m_cameraEventQueue.push(CameraShakeEvent{ e.intensity, e.duration });
			}
		}

		void handlePlayerAttacked(const Domain::Events::PlayerAttackedEvent& e)
		{
			m_cameraEventQueue.push(CameraShakeEvent{ e.intensity,e.duration });
			m_cameraEventQueue.push(CameraZoomEvent{ e.zoom, e.duration });
		}

		//プレイヤーが落ちた時
		void handlePlayerFallOut(const Domain::Events::PlayerFallOutEvent& e)
		{
			m_cameraEventQueue.push(CameraShakeEvent{ e.intensity,e.duration });
			m_cameraEventQueue.push(CameraZoomEvent{ e.zoom, e.duration });

			Vec2 pos = { e.pos.x,e.pos.y + 100 };
			m_effectEventQueue.push(FallDeathEffectEvent{
				pos,
				Vec2::Up(),
				1000.0,
				1.0,
				980,
				360,
				Palette::Crimson,
				true
			});
		}

		void handlePlayerChokerSkilled(const Domain::Events::PlayerChokerSkillEvent& e)
		{
			m_cameraEventQueue.push(CameraZoomEvent{ e.zoom,e.duration });
		}

		void handlePlayerDamaged(const Domain::Events::PlayerDamagedEvent& e)
		{
			Jam::UseCase::AttackProcessor::getInstance().executeAttack(e.attacker, e.target, e.damageInfo);

			// プレイヤーがダメージを受けた時の演出
			m_cameraEventQueue.push(CameraShakeEvent{ e.intensity,e.duration });
		}

		void handlePlayerDeath(const Domain::Events::PlayerDeathEvent& e)
		{
			Jam::Foundation::CoreManager::Instance().setDied(true);
			playerDeath();
			// プレイヤーがダメージを受けた時の演出
			m_cameraEventQueue.push(CameraShakeEvent{ e.intensity, e.duration });
			m_cameraEventQueue.push(CameraZoomEvent{ e.zoom, e.duration });
		}

		Jam::Util::Task playerDeath()
		{
			co_await Jam::Util::WaitSeconds(2.0);
			if (m_onPlayerDeath) m_onPlayerDeath();
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

		void handleEnemySpawned(const Domain::Events::EnemySpawnedEvent& e)
		{
			if (m_enemyManager && e.enemy)
			{
				// EnemyManagerに敵を追加
				int id = m_enemyManager->AddEnemy(e.enemy, e.animationPath);

				// アニメーションの初期設定
				m_enemyManager->getAnimator(id).AddCondition({ { {U"isRunning", false} }, U"Idle", 0 });
				m_enemyManager->getAnimator(id).SetBool(U"isRunning", false);
			}
		}
		void handleExplosion(const Domain::Events::ExplosionEvent& e)
		{
			m_effectEventQueue.push(ExplosionEffectEvent{
				e.position,e.color,e.radius,e.duration,e.particleCount
			});
		}
	};
}
