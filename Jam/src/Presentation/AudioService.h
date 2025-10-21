#pragma once
#include "../Infrastructure/AudioLoader.h"
#include <unordered_map>

namespace Jam::Presentation
{
	class AudioService
	{
	public:
		enum class Sound
		{
			SE_Jump,
			SE_Attack,
			SE_Choker,
			BGM_Title,
			BGM_Game,
		};

		static AudioService& get()
		{
			static AudioService instance;
			return instance;
		}


		void init()
		{
			String samePath = U"../Assets/Sounds/";
			Jam::Presentation::AudioService::get().load(Jam::Presentation::AudioService::Sound::SE_Jump, samePath + U"se_jump.mp3");
			Jam::Presentation::AudioService::get().load(Jam::Presentation::AudioService::Sound::SE_Choker, samePath + U"se_choker.mp3");
			Jam::Presentation::AudioService::get().load(Jam::Presentation::AudioService::Sound::BGM_Title, samePath + U"bgm_title.mp3");

		}

		void load(Sound sound, const s3d::FilePathView& path, bool streaming = false)
		{
			if (m_audios.find(sound) != m_audios.end()) return;

			auto audio = Jam::Infrastructure::AudioLoader::get().load(path, streaming);
			m_audios[sound] = audio;
		}

		void play(Sound sound, bool loop = true, double volume = 0.05)
		{
			auto it = m_audios.find(sound);
			if (it == m_audios.end()) return;

			if (isBGM(sound) && m_currentBGM && m_currentBGM != it->second)
				m_currentBGM->stop();

			if (isBGM(sound))
			{
				m_currentBGM = it->second;
				m_currentBGM->setVolume(volume);
			}
			if (loop) it->second->setLoop(true);
			it->second->play();
		}

		void playOneShot(Sound sound, double volume = 1.0, double pan = 0.0, double speed = 1.0)
		{
			auto it = m_audios.find(sound);
			if (it != m_audios.end())
				it->second->playOneShot(volume, pan, speed);
		}

		void setVolume(double volume)
		{
			if (!m_currentBGM)return;
			m_currentBGM->setVolume(volume);
		}

	private:
		AudioService() = default;

		bool isBGM(Sound sound) const
		{
			switch (sound)
			{
			case Sound::BGM_Title:
			case Sound::BGM_Game:
				return true;
			default:
				return false;
			}
		}

		std::unordered_map<Sound, std::shared_ptr<s3d::Audio>> m_audios;
		std::shared_ptr<s3d::Audio> m_currentBGM = nullptr;
	};
}
