#pragma once
#include <Siv3D.hpp>
#include <unordered_map>

namespace Jam::Presentation
{
	class TextureManager
	{
	private:
		static inline std::unordered_map<String, Texture> s_cache;

	public:
		static const Texture& Load(const String& path)
		{
			// キャッシュに存在すれば再利用
			if (s_cache.contains(path))
			{
				return s_cache[path];
			}

			// 新規読み込み
			s_cache[path] = Texture(Resource(path));
			return s_cache[path];
		}

		static void Clear()
		{
			s_cache.clear();
		}

		static size_t GetCacheSize()
		{
			return s_cache.size();
		}
	};
}
