#include "Infrastructure/AudioLoader.h"

namespace Jam::Infrastructure
{
	AudioLoader& AudioLoader::get()
	{
		static AudioLoader instance;
		return instance;
	}

	std::shared_ptr<s3d::Audio> AudioLoader::load(const s3d::FilePathView& path, bool streaming)
	{
		const std::string key = s3d::Unicode::ToUTF8(path);
		auto it = m_cache.find(key);
		if (it != m_cache.end())
			return it->second;

		auto audio = std::make_shared<s3d::Audio>(path, streaming);
		audio->setLoop(false);
		m_cache[key] = audio;
		return audio;
	}
}
