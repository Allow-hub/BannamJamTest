#pragma once
#include "Siv3DAudio.h"
#include <unordered_map>
#include <memory>

namespace Jam::Infrastructure
{
	class AudioLoader
	{
	public:
		static AudioLoader& get()
		{
			static AudioLoader instance;
			return instance;
		}

		std::shared_ptr<s3d::Audio> load(const s3d::FilePathView& path, bool streaming = false)
		{
			const std::string key = s3d::Unicode::ToUTF8(path);
			auto it = m_cache.find(key);
			if (it != m_cache.end())
				return it->second;

			auto audio = std::make_shared<s3d::Audio>(path, streaming);
			m_cache[key] = audio;
			return audio;
		}

	private:
		AudioLoader() = default;
		std::unordered_map<std::string, std::shared_ptr<s3d::Audio>> m_cache;
	};
}
