#pragma once
#include <Siv3D.hpp>

namespace Jam::Infrastructure
{
	class Siv3DAudio
	{
	public:
		Siv3DAudio() = default;
		explicit Siv3DAudio(const s3d::FilePathView& path, bool streaming = false)
		{
			if (streaming)
				m_audio = std::make_shared<s3d::Audio>(s3d::Audio::Stream, path);
			else
				m_audio = std::make_shared<s3d::Audio>(path);
		}

		void play() { if (m_audio) m_audio->play(); }
		void stop() { if (m_audio) m_audio->stop(); }
		void setVolume(double v) { if (m_audio) m_audio->setVolume(v); }
		void setPan(double p) { if (m_audio) m_audio->setPan(p); }

		bool isPlaying() const { return m_audio && m_audio->isPlaying(); }

	private:
		std::shared_ptr<s3d::Audio> m_audio;
	};
}
