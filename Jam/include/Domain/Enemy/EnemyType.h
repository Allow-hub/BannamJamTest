#pragma once
#include <Siv3D.hpp>

namespace Jam::Domain
{
	enum class EnemyType
	{
		LittleDevil,
		Ribbon,
		GothicLolitaDoll,
		Spider,
		Eye,
		Clown,
		PoisonPlant,
		ParalysisPlant,
		Boss1_3
	};

	// 敵の種類を文字列に変換
	inline String enemyTypeToString(EnemyType type)
	{
		switch (type)
		{
		case EnemyType::LittleDevil: return U"LittleDevil";
		case EnemyType::Ribbon: return U"Ribbon";
		case EnemyType::GothicLolitaDoll: return U"GothicLolitaDoll";
		case EnemyType::Spider: return U"Spider";
		case EnemyType::Eye: return U"Eye";
		case EnemyType::Clown: return U"Clown";
		case EnemyType::PoisonPlant: return U"PoisonPlant";
		case EnemyType::ParalysisPlant: return U"ParalysisPlant";
		case EnemyType::Boss1_3: return U"Boss1_3";
		default: return U"LittleDevil";
		}
	}

	// 文字列を敵の種類に変換
	inline EnemyType stringToEnemyType(const String& str)
	{
		if (str == U"LittleDevil") return EnemyType::LittleDevil;
		if (str == U"Ribbon") return EnemyType::Ribbon;
		if (str == U"GothicLolitaDoll") return EnemyType::GothicLolitaDoll;
		if (str == U"Spider") return EnemyType::Spider;
		if (str == U"Eye") return EnemyType::Eye;
		if (str == U"Clown") return EnemyType::Clown;
		if (str == U"PoisonPlant") return EnemyType::PoisonPlant;
		if (str == U"ParalysisPlant") return EnemyType::ParalysisPlant;
		if (str == U"Boss1_3") return EnemyType::Boss1_3;
		return EnemyType::LittleDevil;
	}
}
