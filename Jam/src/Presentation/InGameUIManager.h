#pragma once
#include "../Domain/Player/Player.h"
#include <Siv3D.hpp>

namespace Jam::Presentation
{
	class InGameUIManager
	{
	private:
		const std::shared_ptr<Jam::Domain::Player::Player>& m_player;
		s3d::Texture m_hpBack;
		s3d::Texture m_hpFront;
		double lastHp = 0.0;

		s3d::Timer damageFaceTimer{ 1.0s };
		bool isDamaged = false;
		int randomFace = 0;

		// 顔アイコン
		s3d::Texture m_faceNormal;
		s3d::Texture m_faceWarning;
		s3d::Texture m_faceDanger;
		s3d::Texture m_faceDamage01;
		s3d::Texture m_faceDamage02;

		s3d::Vec2 m_position = { 300, 10 }; // HPバー描画位置
		s3d::Vec2 m_barSize = { 400, 24 };
		double m_scale = 0.5;

		// 顔アイコン描画位置
		s3d::Vec2 m_facePosition = { 10, 10 };
		s3d::Vec2 m_faceSize = { 510, 510 }; // 顔アイコンサイズ

	public:
		InGameUIManager(const std::shared_ptr<Jam::Domain::Player::Player>& player)
			: m_player(player)
		{
			m_hpBack = Texture(U"../Assets/UI/hp_back.png");
			m_hpFront = Texture(U"../Assets/UI/hp_front.png");

			m_faceNormal = Texture(U"../Assets/UI/face_happy.png");
			m_faceWarning = Texture(U"../Assets/UI/face_normal.png");
			m_faceDanger = Texture(U"../Assets/UI/face_sad.png");

			m_faceDamage01 = Texture(U"../Assets/UI/face_damage_01.png");
			m_faceDamage02 = Texture(U"../Assets/UI/face_damage_02.png");

			m_player->SetOnDamagedCallback
			(
				[this](void)
				{
						this->OnPlayerDamaged();
				}
			);
		}

		void update()
		{
			double currentHp = m_player->getHp();
			if (lastHp != currentHp)
			{
				lastHp = currentHp;
				// HP変化時エフェクトやサウンドなど追加可能
			}

			//ダメージ顔アイコンタイマー処理
			if (damageFaceTimer.reachedZero())
			{
				isDamaged = false;
			}
		}

		void draw() const
		{
			const s3d::Vec2 drawSize = m_barSize * m_scale;

			// 背景を描画
			m_hpBack.scaled(m_scale).draw(m_position);

			double hpRatio = s3d::Clamp(m_player->getHp() / m_player->getMaxHp(), 0.0, 1.0);
			if (hpRatio > 0.0)
			{
				// HPバーの幅を比率で計算
				int displayWidth = static_cast<int>(m_hpFront.width() * hpRatio);
				m_hpFront(0, 0, displayWidth, m_hpFront.height()).scaled(m_scale).draw(m_position);
			}

			const s3d::Texture* faceTex = nullptr;
			if(isDamaged)
			{
				// ダメージ顔アイコンアニメーション
				if (randomFace)
				{
					faceTex = &m_faceDamage01;
				}
				else
				{
					faceTex = &m_faceDamage02;
				}
			}
			else
			{
				// HP比率で顔アイコン切替
				if (hpRatio > 0.6) {
					faceTex = &m_faceNormal;
				}
				else if (hpRatio > 0.3) {
					faceTex = &m_faceWarning;
				}
				else {
					faceTex = &m_faceDanger;
				}
			}

			// 左上に描画
			if (faceTex)
			{
				faceTex->scaled(m_faceSize / faceTex->size()).draw(m_facePosition);
			}
		}

		// 描画位置変更可能
		void setPosition(const s3d::Vec2& pos) { m_position = pos; }
		void setBarSize(const s3d::Vec2& size) { m_barSize = size; }
		void setScale(double scale) { m_scale = scale; }
		void setFacePosition(const s3d::Vec2& pos) { m_facePosition = pos; }
		void setFaceSize(const s3d::Vec2& size) { m_faceSize = size; }

		// ダメージ時の処理
		void OnPlayerDamaged()
		{
			randomFace = s3d::Random(0, 1);
			isDamaged = true;
			damageFaceTimer.restart();
		}
	};
}
