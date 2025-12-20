#include "UseCase/GameEventHandler.h"
#include "UseCase/AttackProcessor.h"
#include "Presentation/AudioService.h"
#include "Presentation/EnemyManager.h"
#include "Domain/Enemy/Missile.h"
#include "Infrastructure/IndependentObjectFactory.h"
#include "Infrastructure/FactoryServiceLocator.h"
#include "Foundation/CoreManager.h"
#include "Domain/Player/Player.h"

using namespace Jam;

namespace Jam::UseCase
{
	GameEventHandler::GameEventHandler(
		Domain::Events::GameEventQueue& gameEventQueue,
		CameraEventQueue& cameraEventQueue,
		EffectEventQueue& effectEventQueue,
		std::function<void()> onNextScene,
		Jam::Presentation::EnemyManager* enemyManager)
		: m_gameEventQueue(gameEventQueue)
		, m_cameraEventQueue(cameraEventQueue)
		, m_effectEventQueue(effectEventQueue)
		, m_onNextScene(onNextScene)
		, m_enemyManager(enemyManager)
	{
	}

	void GameEventHandler::processEvents()
	{
		while (!m_gameEventQueue.empty())
		{
			auto event = m_gameEventQueue.pop();

			std::visit([this](auto&& e)
			{
				using T = std::decay_t<decltype(e)>;

					if constexpr (std::is_same_v<T, Domain::Events::EnemyDamagedEvent>)
						handleEnemyDamaged(e);
					else if constexpr (std::is_same_v<T, Domain::Events::EnemyDefeatedEvent>)
						handleEnemyDefeated(e);
					else if constexpr (std::is_same_v<T, Domain::Events::PlayerAttackedEvent>)
						handlePlayerAttacked(e);
					else if constexpr (std::is_same_v<T, Domain::Events::PlayerChokerSkillEvent>)
						handlePlayerChokerSkilled(e);
					else if constexpr (std::is_same_v<T, Domain::Events::PlayerDamagedEvent>)
						handlePlayerDamaged(e);
					else if constexpr (std::is_same_v<T, Domain::Events::PlayerStatusAilmentEvent>)
						handlePlayerStatusAilment(e);
					else if constexpr (std::is_same_v<T, Domain::Events::PlayerDeathEvent>)
						handlePlayerDeath(e);
					else if constexpr (std::is_same_v<T, Domain::Events::PlayerFallOutEvent>)
						handlePlayerFallOut(e);
					else if constexpr (std::is_same_v<T, Domain::Events::BossAppearedEvent>)
						handleBossAppeared(e);
					else if constexpr (std::is_same_v<T, Domain::Events::ItemCollectedEvent>)
						handleItemCollected(e);
					else if constexpr (std::is_same_v<T, Domain::Events::EnemySpawnedEvent>)
						handleEnemySpawned(e);
					else if constexpr (std::is_same_v<T, Domain::Events::ExplosionEvent>)
						handleExplosion(e);
					else if constexpr (std::is_same_v<T, Domain::Events::BarrierShatteredEvent>)
						handleBarrierShattered(e);
				}, event);
		}
	}

	void GameEventHandler::handleEnemyDamaged(const Domain::Events::EnemyDamagedEvent& e)
	{
		UseCase::AttackProcessor::getInstance().executeAttack(e.attacker, e.target, e.damageInfo);

		Vec2 randomOffset{ Random(-2.0, 2.0), Random(-2.0, 2.0) };
		String text = Format(e.damageInfo.amount);
		m_effectEventQueue.push(TextEffectEvent{ e.damageInfo.position + randomOffset, text, Palette::Green });
		m_effectEventQueue.push(ParticleEffectEvent{
			e.damageInfo.position, e.damageInfo.direction, Palette::Hotpink, 100, 500 });
	}

	void GameEventHandler::handleEnemyDefeated(const Domain::Events::EnemyDefeatedEvent& e)
	{
		if (e.isBoss)
		{
			m_cameraEventQueue.push(CameraShakeEvent{ e.intensity, e.duration });
			Foundation::CoreManager::Instance().setClear(true);
			Foundation::CoreManager::Instance().addFlagment(3);
			if (m_onNextScene) m_onNextScene();
		}
		else
		{
			m_cameraEventQueue.push(CameraShakeEvent{ e.intensity, e.duration });
		}
	}

	void GameEventHandler::handlePlayerAttacked(const Domain::Events::PlayerAttackedEvent& e)
	{
		m_cameraEventQueue.push(CameraShakeEvent{ e.intensity, e.duration });
		m_cameraEventQueue.push(CameraZoomEvent{ e.zoom, e.duration });
	}

	void GameEventHandler::handlePlayerFallOut(const Domain::Events::PlayerFallOutEvent& e)
	{
		m_cameraEventQueue.push(CameraShakeEvent{ e.intensity, e.duration });
		m_cameraEventQueue.push(CameraZoomEvent{ e.zoom, e.duration });

		Vec2 pos = { e.pos.x, e.pos.y + 100 };
		m_effectEventQueue.push(FallDeathEffectEvent{
			pos, Vec2::Up(), 1000.0, 1.0, 980, 360, Palette::Crimson, true });
	}

	void GameEventHandler::handlePlayerChokerSkilled(const Domain::Events::PlayerChokerSkillEvent& e)
	{
		m_cameraEventQueue.push(CameraZoomEvent{ e.zoom, e.duration });
		m_effectEventQueue.push(ChokerSkillEffectEvent{
			e.position, e.direction });
	}

	void GameEventHandler::handlePlayerDamaged(const Domain::Events::PlayerDamagedEvent& e)
	{
		UseCase::AttackProcessor::getInstance().executeAttack(e.attacker, e.target, e.damageInfo);
		m_cameraEventQueue.push(CameraShakeEvent{ e.intensity, e.duration });
	}

	void GameEventHandler::handlePlayerStatusAilment(const Domain::Events::PlayerStatusAilmentEvent& e)
	{
		// AttackProcessor に登録されている Player を取得して状態異常を付与する
		auto attacker = e.attacker; // 未使用だが将来的な拡張のため保持
		auto target = e.target;

		// Player は ITakeDamageable として登録されているので dynamic_pointer_cast
		auto damageable = UseCase::AttackProcessor::getInstance().getDamageable(target);
		if (damageable)
		{
			// 実体が Playerか確認
			auto player = std::dynamic_pointer_cast<Domain::Player::Player>(damageable);
			if (player)
			{
				player->applyStatusAilment(e.type, e.duration, e.power, e.tickInterval);

				// 状態異常適用時の視覚効果（色分け）
				const Vec2 pos = player->getPosition();
				ColorF color = Palette::White;
				switch (e.type)
				{
				case Domain::Player::StatusAilmentType::Poison:
					color = ColorF{0.6,0.3,0.6,0.9 }; // 紫
					break;
				case Domain::Player::StatusAilmentType::Paralysis:
					color = ColorF{1.0,1.0,0.0,0.9 }; // 黄
					break;
				default:
					break;
				}

				m_effectEventQueue.push(RingEffectEvent{ pos, color,80.0,0.35,5.0 });
				m_effectEventQueue.push(ParticleEffectEvent{ pos, Vec2::Zero(), color,25,180.0,0.5, true, false });
			}
		}
	}

	void GameEventHandler::handlePlayerDeath(const Domain::Events::PlayerDeathEvent& e)
	{
		Foundation::CoreManager::Instance().setDied(true);
		playerDeath();
		m_cameraEventQueue.push(CameraShakeEvent{ e.intensity, e.duration });
		m_cameraEventQueue.push(CameraZoomEvent{ e.zoom, e.duration });
	}

	Jam::Util::Task GameEventHandler::playerDeath()
	{
		co_await Jam::Util::WaitSeconds(2.0);
		if (m_onNextScene) m_onNextScene();
	}

	void GameEventHandler::handleBossAppeared(const Domain::Events::BossAppearedEvent& e)
	{
		m_cameraEventQueue.push(CameraFocusEvent{ e.position, 2.5, 0.8 });
	}

	void GameEventHandler::handleItemCollected(const Domain::Events::ItemCollectedEvent& e)
	{
		if (e.isRare)
			m_cameraEventQueue.push(CameraZoomEvent{ 1.3, 0.5 });
	}

	void GameEventHandler::handleEnemySpawned(const Domain::Events::EnemySpawnedEvent& e)
	{
		if (m_enemyManager && e.enemy)
		{
			int id = m_enemyManager->AddEnemy(e.enemy, e.animationPath);
			m_enemyManager->getAnimator(id).addCondition({ { {U"isRunning", false} }, U"Idle", 0 });
			m_enemyManager->getAnimator(id).setBool(U"isRunning", false);
		}
	}

	void GameEventHandler::handleExplosion(const Domain::Events::ExplosionEvent& e)
	{
		m_effectEventQueue.push(ExplosionEffectEvent{
			e.position, e.color, e.radius, e.duration, e.particleCount });
	}

	void GameEventHandler::handleBarrierShattered(const Domain::Events::BarrierShatteredEvent& e)
	{
		// バリア破壊時のSEを再生
		Presentation::AudioService::get().playOneShot(
			Presentation::AudioService::Sound::SE_BarrierBreak, 1.0);

		// ガラス破壊エフェクトを発行
		m_effectEventQueue.push(GlassShatterEffectEvent{
			e.position, e.impactDirection,
			ColorF{ 0.5, 0.8, 1.0, 0.7 },
			25, 400.0, 1.5, e.barrierRadius
		});

		m_effectEventQueue.push(RingEffectEvent{
			e.position, ColorF{ 0.7, 0.9, 1.0, 0.8 },
			200.0, 0.5, 8.0
		});
	}
}
