#pragma once
#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include "TitleScene.h"
#include "../PlayerManager.h"
#include "../../UseCase/PlayerService.h"
#include "../../Infrastructure/Siv3DInputManager.h"
#include "../../Infrastructure/Siv3DPhysicsBody.h"
#include "../../Domain/Stage/Stage.h"

namespace Jam::Presentation::Scenes
{
	using App = SceneManager<String>;

	class GameScene : public App::Scene
	{
	private:
		P2World m_world;
		double m_accumulatedTime = 0.0;//2D 物理演算のシミュレーション蓄積時間（秒）
		std::shared_ptr<Domain::Player> m_player;
		Jam::UseCase::PlayerService m_playerService;
		Jam::Presentation::PlayerManager m_playerManager;
		Jam::Infrastructure::Siv3DInputManager m_inputManager;
		// デバッグ用の床
		std::shared_ptr<Infrastructure::Physics::Siv3DPhysicsBody> m_ground;
		
		// Stage テスト用
		std::unique_ptr<Jam::Domain::Stage::Stage> m_stage;

	public:
		GameScene(const InitData& init)
			: IScene{ init },
			m_world({ 0, 980 }),//引数は重力
			m_inputManager(),
			m_player(std::make_shared<Domain::Player>(
				std::make_shared<Infrastructure::Physics::Siv3DPhysicsBody>(
					m_world, Vec2{ 100,300 }, SizeF{ 50, 80 }
				)
			)),
			m_playerService(m_player, m_inputManager),
			m_playerManager(m_player),
			m_stage(std::make_unique<Jam::Domain::Stage::Stage>())
		{
			m_ground = std::make_shared<Infrastructure::Physics::Siv3DPhysicsBody>(
			m_world,
			Vec2{ 640, 700 },        // 中心座標
			SizeF{ 1280, 40 },       // 幅・高さ
			s3d::P2BodyType::Static   // 動かない床
			);
			
			// Stage JSONファイルを読み込み
			if (!m_stage->loadFromJson(U"App/Stage/stage1.json"))
			{
				Console << U"Stage JSONファイルの読み込みに失敗しました";
			}
		}

		void update() override
		{
			// 入力→PlayerService更新→Playerの状態更新
			m_playerService.update(Scene::DeltaTime());

			// 累積時間で固定ステップ物理
			constexpr double StepTime = 1.0 / 200.0;
			m_accumulatedTime += Scene::DeltaTime();

			while (StepTime <= m_accumulatedTime)
			{
				m_world.update(StepTime);   // 物理演算を StepTime 秒進める
				m_accumulatedTime -= StepTime;
			}

		}

		void draw() const override
		{
			Scene::SetBackground(ColorF{ 0.9, 0.9, 1.0 });
			RectF{ 0, 680, 1280, 40 }.draw(Palette::Gray);

			// Stageの描画
			if (m_stage)
			{
				m_stage->draw();
			}

			// Playerの描画
			m_playerManager.draw();

			// Stageや敵も描画
		}
	};
}
