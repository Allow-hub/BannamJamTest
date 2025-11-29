#pragma once
#include "Siv3DAudio.h"
#include <unordered_map>
#include <memory>
#include <string>

namespace Jam::Infrastructure
{
	//音をロード
	class AudioLoader
	{
	public:
		static AudioLoader& get();

		std::shared_ptr<s3d::Audio> load(const s3d::FilePathView& path, bool streaming = false);

	private:
		AudioLoader() = default;

		std::unordered_map<std::string, std::shared_ptr<s3d::Audio>> m_cache;
	};
}
