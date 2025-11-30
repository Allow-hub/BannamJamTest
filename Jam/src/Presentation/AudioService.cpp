#include "Presentation/AudioService.h"

using namespace s3d;

namespace Jam::Presentation
{
	AudioService& AudioService::get()
	{
		static AudioService instance;
		return instance;
	}

	void AudioService::init()
	{
		String basePath = U"Assets/Sounds/";

		load(Sound::SE_Jump, basePath + U"se_jump.mp3");
		load(Sound::SE_Choker, basePath + U"se_chokerThrow.mp3");
		load(Sound::SE_GetFlagment, basePath + U"se_getFlagment.mp3");
		load(Sound::SE_Damage, basePath + U"se_damage.mp3");
		load(Sound::SE_Explosion, basePath + U"se_explosion.mp3");
		load(Sound::SE_Goal, basePath + U"se_goal.mp3");
		load(Sound::SE_Button, basePath + U"se_button.mp3");
		load(Sound::SE_EnemyHit, basePath + U"se_attack.mp3");
		load(Sound::SE_BarrierBreak, basePath + U"se_barrier.mp3");
		load(Sound::SE_FallDamage, basePath + U"se_fallDamage.mp3");
		load(Sound::SE_BossDown, basePath + U"se_bossDown.mp3");

		load(Sound::BGM_Title, basePath + U"bgm_title.mp3");
		load(Sound::BGM_Story1, basePath + U"bgm_story1_1.mp3");
		load(Sound::BGM_Story2, basePath + U"bgm_hauntedHouse.mp3");
		load(Sound::BGM_Game, basePath + U"bgm_ingame.mp3");
	}

	void AudioService::load(Sound sound, const FilePathView& path, bool streaming)
	{
		if (m_audios.contains(sound))
			return;

		auto audio = Jam::Infrastructure::AudioLoader::get().load(path, streaming);
		m_audios[sound] = audio;
	}

	void AudioService::play(Sound sound, bool loop, double baseVolume)
	{
		auto it = m_audios.find(sound);
		if (it == m_audios.end()) return;

		// BGM処理
		if (isBGM(sound))
		{
			if (m_currentBGM && m_currentBGM != it->second)
			{
				m_currentBGM->stop();
			}

			m_currentBGM = it->second;
			double finalVolume = m_masterVolume * m_bgmVolume * baseVolume;
			m_currentBGM->setVolume(finalVolume);
			m_currentBGM->setLoop(loop);
			m_currentBGM->play();
		}
		else
		{
			double finalVolume = m_masterVolume * m_seVolume * baseVolume;
			it->second->setVolume(finalVolume);
			it->second->setLoop(loop);
			it->second->play();
		}
	}

	void AudioService::playOneShot(Sound sound, double volume, double pan, double speed)
	{
		auto it = m_audios.find(sound);
		if (it == m_audios.end()) return;

		double finalVolume = m_masterVolume * m_seVolume * volume;
		it->second->playOneShot(finalVolume, pan, speed);
	}

	void AudioService::setMasterVolume(double volume)
	{
		m_masterVolume = Math::Clamp(volume, 0.0, 1.0);
		updateAllVolumes();
	}

	void AudioService::setSEVolume(double volume)
	{
		m_seVolume = Math::Clamp(volume, 0.0, 1.0);
	}

	void AudioService::setBGMVolume(double volume)
	{
		m_bgmVolume = Math::Clamp(volume, 0.0, 1.0);
		updateBGMVolume();
	}

	double AudioService::getMasterVolume() const { return m_masterVolume; }
	double AudioService::getSEVolume() const { return m_seVolume; }
	double AudioService::getBGMVolume() const { return m_bgmVolume; }

	void AudioService::stopBGM()
	{
		if (m_currentBGM)
		{
			m_currentBGM->stop();
			m_currentBGM = nullptr;
		}
	}

	void AudioService::stopAll()
	{
		for (auto& [sound, audio] : m_audios)
		{
			audio->stop();
		}
		m_currentBGM = nullptr;
	}

	bool AudioService::isBGM(Sound sound) const
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

	void AudioService::updateBGMVolume()
	{
		if (m_currentBGM)
		{
			double finalVolume = m_masterVolume * m_bgmVolume;
			m_currentBGM->setVolume(finalVolume);
		}
	}

	void AudioService::updateAllVolumes()
	{
		updateBGMVolume();
	}
}
