#pragma once
#include "../Infrastructure/AudioLoader.h"
#include <unordered_map>
#include <memory>

namespace Jam::Presentation
{
	class AudioService
	{
	public:
		// === 定数 ===
		static constexpr double VOLUME_BUTTON = 1.0;

		// === サウンド識別子 ===
		enum class Sound
		{
			SE_Jump,
			SE_Attack,
			SE_Choker,
			SE_GetFlagment,
			SE_Damage,
			SE_Explosion,
			SE_Goal,
			SE_Button,
			SE_EnemyHit,
			SE_BarrierBreak,
			SE_FallDamage,
			SE_BossDown,
			BGM_Title,
			BGM_Story1,
			BGM_Story2,
			BGM_Story3,
			BGM_Game,
		};

		// === シングルトン取得 ===
		static AudioService& get();

		// === 初期化 ===
		void init();

		// === 読み込み・再生 ===
		void load(Sound sound, const s3d::FilePathView& path, bool streaming = false);
		void play(Sound sound, bool loop = true, double baseVolume = 1.0);
		void playOneShot(Sound sound, double volume = 1.0, double pan = 0.0, double speed = 1.0);

		// === ボリューム設定 ===
		void setMasterVolume(double volume);
		void setSEVolume(double volume);
		void setBGMVolume(double volume);

		// === ボリューム取得 ===
		[[nodiscard]] double getMasterVolume() const;
		[[nodiscard]] double getSEVolume() const;
		[[nodiscard]] double getBGMVolume() const;

		// === 停止操作 ===
		void stopBGM();
		void stopAll();

	private:
		AudioService() = default;

		bool isBGM(Sound sound) const;
		void updateBGMVolume();
		void updateAllVolumes();

		std::unordered_map<Sound, std::shared_ptr<s3d::Audio>> m_audios;
		std::shared_ptr<s3d::Audio> m_currentBGM = nullptr;

		double m_masterVolume = 1.0;
		double m_seVolume = 1.0;
		double m_bgmVolume = 0.05;
	};
}
