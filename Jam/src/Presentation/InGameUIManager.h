#pragma once
#include <Siv3D.hpp>
#include <memory>

namespace Jam::Domain::Player
{
	class Player; // 前方宣言（ヘッダ依存を最小化）
}

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
		s3d::Texture m_fragmentMemory_00;
		s3d::Texture m_fragmentMemory_01;
		s3d::Texture m_fragmentMemory_02;
		s3d::Texture m_fragmentMemory_03;

		// 左上ミニキャラ顔
		s3d::Texture m_faceNormal;
		s3d::Texture m_faceWarning;
		s3d::Texture m_faceDanger;
		s3d::Texture m_hand;

		// backgroundと左下ダメージキャラ
		s3d::Texture m_backgroundNoDamge;
		s3d::Texture m_backgroundDamage_00;
		s3d::Texture m_backgroundDamage_01;

		s3d::Vec2 m_position = { -10, 10 };
		double m_scale = 1.0;

		s3d::Vec2 m_facePosition = { 20, 230 };
		s3d::Vec2 m_faceSize = { 260, 200 };
		s3d::Vec2 m_fragmentPosition = { 24, 148 };

	public:
		InGameUIManager(const std::shared_ptr<Jam::Domain::Player::Player>& player);

		void update();
		void draw() const;

	private:
		void drawShadowedTexture(const s3d::Texture& tex, const s3d::Vec2& pos, const s3d::Vec2& size) const;
		void onPlayerDamaged();
	};
}
