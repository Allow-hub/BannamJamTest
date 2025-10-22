#pragma once
#include "IEnemyAI.h"

namespace Jam::Domain::Enemy
{
	//標的を探して一定位置を移動
	struct PatrolAI : IEnemyAI
	{
		enum MoveState
		{
			MoveRight = 0,
			MoveLeft
		};
		MoveState moveState;

		Vec2 m_startPos;
		Vec2 m_endPos;
		int m_direction = 1;

		//コンストラクタ。巡回するエリアの中心地と範囲を設定します。
		//Enemyの具象クラスでGetPotision()と、巡回するエリアの半径の値を渡す必要があります。
		PatrolAI(const Vec2& centerPos, double patrolRadius)
		{
			// 受け取った中心位置と範囲から、巡回の開始・終了地点を計算して保存する
			m_startPos = Vec2{ centerPos.x - patrolRadius, centerPos.y };
			m_endPos = Vec2{ centerPos.x + patrolRadius, centerPos.y };
			Print << m_startPos;
			Print << m_endPos;
		}

		void enter(EnemyBase& enemy) override
		{
			moveState = MoveLeft;
			enemy.onAIEvent(EnemyAIEvent::PlayerFound);
			// フック
			enemy.onPatrolEnter();
		}

		void update(EnemyBase& enemy, double deltaTime) override
		{
			//移動方向切り替え
			if (enemy.getPosition().x <= m_startPos.x)
			{

				moveState = MoveRight;
			}
			else if(enemy.getPosition().x >= m_endPos.x)
			{
				moveState = MoveLeft;
			}

			//移動
			switch (moveState)
			{
			case MoveRight:
			{
				enemy.moveRight();
			}break;
			case MoveLeft:
			{
				enemy.moveLeft();
			}break;
			}
			// フック
			enemy.onPatrolUpdate(deltaTime);
		}

		void exit(EnemyBase& enemy) override
		{
			Print << U"EXIT";
			// フック
			enemy.onPatrolExit();
		}
	};
}
