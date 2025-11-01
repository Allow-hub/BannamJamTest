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

		// 左上記憶のかけらアイコン
		s3d::Texture m_fragmentMemory_00;// 0個
		s3d::Texture m_fragmentMemory_01;// 1個
		s3d::Texture m_fragmentMemory_02;// 2個
		s3d::Texture m_fragmentMemory_03;// 3個

		// 左上ミニキャラ顔
		s3d::Texture m_faceNormal;
		s3d::Texture m_faceWarning;
		s3d::Texture m_faceDanger;
		s3d::Texture m_hand;

		// backgroundと左下ダメージキャラ
		s3d::Texture m_backgroundNoDamge;//ダメージを受けてないとき
		s3d::Texture m_backgroundDamage_00;//ダメージを受けたときの差分１
		s3d::Texture m_backgroundDamage_01;//ダメージを受けたときの差分２

		s3d::Vec2 m_position = { -10, 10 }; // HPバー描画位置
		double m_scale = 1.0;

		// 顔アイコン描画位置
		s3d::Vec2 m_facePosition = { 70, 230 };
		s3d::Vec2 m_faceSize = { 180, 190 }; // 顔アイコンサイズ

		s3d::Vec2 m_fragmentPosition = { 24, 148 }; // m_facePositionからの相対座標

	public:
		InGameUIManager(const std::shared_ptr<Jam::Domain::Player::Player>& player)
			: m_player(player)
		{
			m_hpBack = Texture(U"Assets/UI/hp_back.png");
			m_hpFront = Texture(U"Assets/UI/hp_front.png");

			m_faceNormal = Texture(U"Assets/UI/face_happy.png");
			m_faceWarning = Texture(U"Assets/UI/face_normal.png");
			m_faceDanger = Texture(U"Assets/UI/face_sad.png");
			m_hand = Texture(U"Assets/UI/face_hand.png");

			m_backgroundNoDamge = Texture(U"Assets/UI/frameNoDamage.png");
			m_backgroundDamage_00 = Texture(U"Assets/UI/frameDamage_00.png");
			m_backgroundDamage_01 = Texture(U"Assets/UI/frameDamage_01.png");

			m_fragmentMemory_00 = Texture(U"Assets/UI/fragment_00.png");
			m_fragmentMemory_01 = Texture(U"Assets/UI/fragment_01.png");
			m_fragmentMemory_02 = Texture(U"Assets/UI/fragment_02.png");
			m_fragmentMemory_03 = Texture(U"Assets/UI/fragment_03.png");

			m_player->setOnDamagedCallback
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
			const double w = Scene::Width();
			const double h = Scene::Height();
			const s3d::Texture* background = nullptr;// 現在の左下アイコンテクスチャと背景

			if (isDamaged)
			{
				// ダメージ顔アイコン
				if (randomFace)
					background = &m_backgroundDamage_00;
				else
					background = &m_backgroundDamage_01;
			}
			else
				background = &m_backgroundNoDamge;//普通の顔
			background->resized(w, h).draw();// 背景と顔差分はくっついてる描画

			// HP背景を描画
			m_hpBack.scaled(m_scale).draw(m_position);

			double hpRatio = s3d::Clamp(m_player->getHp() / m_player->getMaxHp(), 0.0, 1.0);
			if (hpRatio > 0.0)
			{
				// HPバーの幅を比率で計算
				int displayWidth = static_cast<int>(m_hpFront.width() * hpRatio);
				m_hpFront(0, 0, displayWidth, m_hpFront.height()).scaled(m_scale).draw(m_position);
			}


			const s3d::Texture* faceTex = nullptr;// 現在の左下アイコンテクスチャ
			const s3d::Texture* fragmentMemoryTex = nullptr;// 現在の左上記憶のかけらの数テクスチャ

			// HP比率で顔アイコン切替
			if (hpRatio > 0.6)
				faceTex = &m_faceNormal;
			else if (hpRatio > 0.3)
				faceTex = &m_faceWarning;
			else
				faceTex = &m_faceDanger;

			// 左上に描画
			if (faceTex)
				drawShadowedTexture(*faceTex, m_facePosition, m_faceSize);

			// 記憶のかけらアイコン切替
			switch (Jam::Foundation::CoreManager::Instance().getFlagment())
			{
				case 0:
					fragmentMemoryTex = &m_fragmentMemory_00;
					break;
				case 1:
					fragmentMemoryTex = &m_fragmentMemory_01;
					break;
				case 2:
					fragmentMemoryTex = &m_fragmentMemory_02;
					break;
				case 3:
					fragmentMemoryTex = &m_fragmentMemory_03;
					break;
			default:
				fragmentMemoryTex = &m_fragmentMemory_03;
				break;
			}
			if (fragmentMemoryTex)
				fragmentMemoryTex->scaled(m_scale).draw(m_position + m_fragmentPosition);

			drawShadowedTexture(m_hand, m_facePosition, m_faceSize);
		}

		void drawShadowedTexture(const Texture& tex, const Vec2& pos, const Vec2& size) const
		{
			// 影のオフセット
			const Vec2 offset = { 1.0, 1.0 };

			// 影の色（透明度高め）
			const ColorF shadowColor{ 0.0, 0.0, 0.0, 0.4 };

			// 拡大して何回か描いてぼかしっぽく
			for (int i = 0; i < 3; ++i)
			{
				double scale = 1.0 + i * 0.02;
				tex.scaled(size / tex.size() * scale)
					.draw(pos + offset, shadowColor);
			}

			// 本体を最後に描く
			tex.scaled(size / tex.size()).draw(pos);
		}


		// ダメージ時の処理
		void OnPlayerDamaged()
		{
			randomFace = s3d::Random(0, 1);
			isDamaged = true;
			damageFaceTimer.restart();
		}
	};
}
