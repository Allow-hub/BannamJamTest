#pragma once
#include <Siv3D.hpp>
#include "../Domain/Events/GameEvents.h"
#include "CameraEvent.h"
#include "EffectEvents.h"
#include "../Foundation/CoroutineUtil.h"

namespace Jam::Presentation { class EnemyManager; }

namespace Jam::UseCase
{
	class GameEventHandler
	{
	private:
		Domain::Events::GameEventQueue& m_gameEventQueue;
		CameraEventQueue& m_cameraEventQueue;
		EffectEventQueue& m_effectEventQueue;
		std::function<void()> m_onNextScene;
		Jam::Presentation::EnemyManager* m_enemyManager = nullptr;

	public:
		GameEventHandler(
			Domain::Events::GameEventQueue& gameEventQueue,
			CameraEventQueue& cameraEventQueue,
			EffectEventQueue& effectEventQueue,
			std::function<void()> onNextScene,
			Jam::Presentation::EnemyManager* enemyManager
		);

		//イベントの実行
		void processEvents();

	private:
		// 個別イベントハンドラ
		//敵にダメージを与えた時
		void handleEnemyDamaged(const Domain::Events::EnemyDamagedEvent& e);

		//敵を倒したとき
		void handleEnemyDefeated(const Domain::Events::EnemyDefeatedEvent& e);

		//プレイヤーが攻撃したとき
		void handlePlayerAttacked(const Domain::Events::PlayerAttackedEvent& e);

		//プレイヤーが落下したとき
		void handlePlayerFallOut(const Domain::Events::PlayerFallOutEvent& e);

		//プレイヤーがチョーカーを投げる時
		void handlePlayerChokerSkilled(const Domain::Events::PlayerChokerSkillEvent& e);

		//プレイヤーがダメージを受けた時
		void handlePlayerDamaged(const Domain::Events::PlayerDamagedEvent& e);

		//プレイヤーが死んだとき
		void handlePlayerDeath(const Domain::Events::PlayerDeathEvent& e);

		//死亡時のディレイ処理
		Jam::Util::Task playerDeath();

		//ボスが現れた時
		void handleBossAppeared(const Domain::Events::BossAppearedEvent& e);

		//アイテムを手に入れた時
		void handleItemCollected(const Domain::Events::ItemCollectedEvent& e);

		//敵がスポーンしたとき
		void handleEnemySpawned(const Domain::Events::EnemySpawnedEvent& e);

		//爆弾が爆発したとき
		void handleExplosion(const Domain::Events::ExplosionEvent& e);

		//シールドを割った時
		void handleBarrierShattered(const Domain::Events::BarrierShatteredEvent& e);
	};
}
