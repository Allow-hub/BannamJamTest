#include "InGameUIManager.h"
#include "../Domain/Player/Player.h"
#include "../Foundation/CoreManager.h"

namespace Jam::Presentation
{
	InGameUIManager::InGameUIManager(const std::shared_ptr<Jam::Domain::Player::Player>& player)
		: m_player(player)
	{
		m_hpBack = Texture(Resource(U"Assets/UI/hp_back.png"));
		m_hpFront = Texture(Resource(U"Assets/UI/hp_front.png"));

		m_faceNormal = Texture(Resource(U"Assets/UI/face_happy.png"));
		m_faceWarning = Texture(Resource(U"Assets/UI/face_normal.png"));
		m_faceDanger = Texture(Resource(U"Assets/UI/face_sad.png"));
		m_hand = Texture(Resource(U"Assets/UI/face_hand.png"));

		m_backgroundNoDamge = Texture(Resource(U"Assets/UI/frameNoDamage.png"));
		m_backgroundDamage_00 = Texture(Resource(U"Assets/UI/frameDamage_00.png"));
		m_backgroundDamage_01 = Texture(Resource(U"Assets/UI/frameDamage_01.png"));

		m_fragmentMemory_00 = Texture(Resource(U"Assets/UI/fragment_00.png"));
		m_fragmentMemory_01 = Texture(Resource(U"Assets/UI/fragment_01.png"));
		m_fragmentMemory_02 = Texture(Resource(U"Assets/UI/fragment_02.png"));
		m_fragmentMemory_03 = Texture(Resource(U"Assets/UI/fragment_03.png"));

		m_player->setOnDamagedCallback([this]() { this->onPlayerDamaged(); });
	}

	void InGameUIManager::update()
	{
		double currentHp = m_player->getHp();
		if (lastHp != currentHp)
		{
			lastHp = currentHp;
			// HP変化時エフェクトやサウンドなど追加可能
		}

		if (damageFaceTimer.reachedZero())
		{
			isDamaged = false;
		}
	}

	void InGameUIManager::draw() const
	{
		const double w = Scene::Width();
		const double h = Scene::Height();
		const Texture* background = nullptr;

		if (isDamaged)
			background = randomFace ? &m_backgroundDamage_00 : &m_backgroundDamage_01;
		else
			background = &m_backgroundNoDamge;

		background->resized(w, h).draw();

		m_hpBack.scaled(m_scale).draw(m_position);

		double hpRatio = Clamp(m_player->getHp() / m_player->getMaxHp(), 0.0, 1.0);
		if (hpRatio > 0.0)
		{
			int displayWidth = static_cast<int>(m_hpFront.width() * hpRatio);
			m_hpFront(0, 0, displayWidth, m_hpFront.height()).scaled(m_scale).draw(m_position);
		}

		const Texture* faceTex = nullptr;
		const Texture* fragmentMemoryTex = nullptr;

		if (hpRatio > 0.6)
			faceTex = &m_faceNormal;
		else if (hpRatio > 0.3)
			faceTex = &m_faceWarning;
		else
			faceTex = &m_faceDanger;

		if (faceTex)
			drawShadowedTexture(*faceTex, m_facePosition, m_faceSize);

		switch (Jam::Foundation::CoreManager::Instance().getFlagment())
		{
		case 0: fragmentMemoryTex = &m_fragmentMemory_00; break;
		case 1: fragmentMemoryTex = &m_fragmentMemory_01; break;
		case 2: fragmentMemoryTex = &m_fragmentMemory_02; break;
		case 3: fragmentMemoryTex = &m_fragmentMemory_03; break;
		default: fragmentMemoryTex = &m_fragmentMemory_03; break;
		}
		if (fragmentMemoryTex)
			fragmentMemoryTex->scaled(m_scale).draw(m_position + m_fragmentPosition);

		drawShadowedTexture(m_hand, m_facePosition, m_faceSize);
	}

	//----------------------------------------
	// 影付き描画
	//----------------------------------------
	void InGameUIManager::drawShadowedTexture(const Texture& tex, const Vec2& pos, const Vec2& size) const
	{
		const Vec2 offset = { 1.0, 1.0 };
		const ColorF shadowColor{ 0.0, 0.0, 0.0, 0.4 };

		for (int i = 0; i < 3; ++i)
		{
			double scale = 1.0 + i * 0.02;
			tex.scaled(size / tex.size() * scale).draw(pos + offset, shadowColor);
		}

		tex.scaled(size / tex.size()).draw(pos);
	}

	//----------------------------------------
	// ダメージ時処理
	//----------------------------------------
	void InGameUIManager::onPlayerDamaged()
	{
		randomFace = Random(0, 1);
		isDamaged = true;
		damageFaceTimer.restart();
	}
}
