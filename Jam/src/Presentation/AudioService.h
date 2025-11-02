#pragma once
#include "../Infrastructure/AudioLoader.h"
#include <unordered_map>

namespace Jam::Presentation
{
	class AudioService
	{
	public:
		// 音量の定数定義
		static constexpr double VOLUME_BUTTON = 1.0;

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
			BGM_Story2,  // 将来用
			BGM_Story3,  // 将来用
			BGM_Game,
		};

		static AudioService& get()
		{
			static AudioService instance;
			return instance;
		}

		void init()
		{
			String samePath = U"Assets/Sounds/";
			load(Sound::SE_Jump, samePath + U"se_jump.mp3");
			load(Sound::SE_Choker, samePath + U"se_chokerThrow.mp3");
			load(Sound::SE_GetFlagment, samePath + U"se_getFlagment.mp3");
			load(Sound::SE_Damage, samePath + U"se_damage.mp3");
			load(Sound::SE_Explosion, samePath + U"se_explosion.mp3");
			load(Sound::SE_Goal, samePath + U"se_goal.mp3");
			load(Sound::SE_Button, samePath + U"se_button.mp3");
			load(Sound::SE_EnemyHit, samePath + U"se_attack.mp3");
			load(Sound::SE_BarrierBreak, samePath + U"se_barrier.mp3");
			load(Sound::SE_FallDamage, samePath + U"se_fallDamage.mp3");
			load(Sound::SE_BossDown, samePath + U"se_bossDown.mp3");
			load(Sound::BGM_Title, samePath + U"bgm_title.mp3");
			load(Sound::BGM_Story1, samePath + U"bgm_story1_1.mp3");
			// 将来的に追加する場合
			 load(Sound::BGM_Story2, samePath + U"bgm_hauntedHouse.mp3");
			// load(Sound::BGM_Story3, samePath + U"bgm_story3.mp3");
			load(Sound::BGM_Game, samePath + U"bgm_ingame.mp3");
		}

		void load(Sound sound, const s3d::FilePathView& path, bool streaming = false)
		{
			if (m_audios.find(sound) != m_audios.end()) return;
			auto audio = Jam::Infrastructure::AudioLoader::get().load(path, streaming);
			m_audios[sound] = audio;
		}

		void play(Sound sound, bool loop = true, double baseVolume = 1.0)
		{
			auto it = m_audios.find(sound);
			if (it == m_audios.end()) return;

			// BGMの場合、前のBGMを停止
			if (isBGM(sound) && m_currentBGM && m_currentBGM != it->second)
			{
				m_currentBGM->stop();
			}

			if (isBGM(sound))
			{
				m_currentBGM = it->second;
				// マスター × BGM ボリュームを適用
				double finalVolume = m_masterVolume * m_bgmVolume * baseVolume;
				m_currentBGM->setVolume(finalVolume);
			}
			else
			{
				// SEの場合、マスター × SE ボリュームを適用
				double finalVolume = m_masterVolume * m_seVolume * baseVolume;
				it->second->setVolume(finalVolume);
			}

			if (loop) it->second->setLoop(true);
			it->second->play();
		}

		void playOneShot(Sound sound, double volume = 1.0, double pan = 0.0, double speed = 1.0)
		{
			auto it = m_audios.find(sound);
			if (it != m_audios.end())
			{
				// マスター × SE ボリュームを適用
				double finalVolume = m_masterVolume * m_seVolume * volume;
				it->second->playOneShot(finalVolume, pan, speed);
			}
		}

		// === ボリューム設定メソッド ===

		// マスターボリュームを設定（0.0 ~ 1.0）
		void setMasterVolume(double volume)
		{
			m_masterVolume = Math::Clamp(volume, 0.0, 1.0);
			updateAllVolumes();
		}

		// SEボリュームを設定（0.0 ~ 1.0）
		void setSEVolume(double volume)
		{
			m_seVolume = Math::Clamp(volume, 0.0, 1.0);
			// 現在再生中のSEには即座に反映されない（次回再生時に適用）
		}

		// BGMボリュームを設定（0.0 ~ 1.0）
		void setBGMVolume(double volume)
		{
			m_bgmVolume = Math::Clamp(volume, 0.0, 1.0);
			updateBGMVolume();
		}

		// === ボリューム取得メソッド ===

		[[nodiscard]] double getMasterVolume() const { return m_masterVolume; }
		[[nodiscard]] double getSEVolume() const { return m_seVolume; }
		[[nodiscard]] double getBGMVolume() const { return m_bgmVolume; }

		// 現在のBGMを停止
		void stopBGM()
		{
			if (m_currentBGM)
			{
				m_currentBGM->stop();
				m_currentBGM = nullptr;
			}
		}

		// すべてのオーディオを停止
		void stopAll()
		{
			for (auto& [sound, audio] : m_audios)
			{
				audio->stop();
			}
			m_currentBGM = nullptr;
		}

	private:
		AudioService() = default;

		bool isBGM(Sound sound) const
		{
			switch (sound)
			{
			case Sound::BGM_Title:
			case Sound::BGM_Story1:
			case Sound::BGM_Story2:
			case Sound::BGM_Story3:
			case Sound::BGM_Game:
				return true;
			default:
				return false;
			}
		}

		// BGMボリュームを更新（再生中のBGMに即座に反映）
		void updateBGMVolume()
		{
			if (m_currentBGM)
			{
				double finalVolume = m_masterVolume * m_bgmVolume;
				m_currentBGM->setVolume(finalVolume);
			}
		}

		// すべてのボリュームを更新
		void updateAllVolumes()
		{
			updateBGMVolume();
			// SEは再生中のものには反映されない（Siv3Dの仕様上、oneShotは変更不可）
		}

		std::unordered_map<Sound, std::shared_ptr<s3d::Audio>> m_audios;
		std::shared_ptr<s3d::Audio> m_currentBGM = nullptr;

		// ボリューム設定（0.0 ~ 1.0）
		double m_masterVolume = 1.0;  // マスターボリューム
		double m_seVolume = 1.0;      // SEボリューム
		double m_bgmVolume = 0.05;    // BGMボリューム（デフォルトを0.05に設定）
	};
}
