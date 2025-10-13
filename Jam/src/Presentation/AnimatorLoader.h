#pragma once
#include <Siv3D.hpp>
#include "Animator.h"

namespace Jam::Presentation
{
	class AnimatorLoader
	{
	public:
		// JSON から Animator にクリップを登録
		// JSON ファイルから Animator のクリップをロードする
		static	bool LoadAnimatorFromJSON(Animator& animator, const String& jsonPath)
		{
			const JSON json = JSON::Load(jsonPath);

			if (not json)
			{
				Console << U"Failed to load JSON: " << jsonPath;
				return false;
			}

			if (json.getType() != JSONValueType::Object)
			{
				Console << U"JSON is not an object: " << jsonPath;
				return false;
			}

			const auto& clipsArray = json[U"clips"];
			if (clipsArray.getType() != JSONValueType::Array)
			{
				Console << U"Missing 'clips' array in JSON";
				return false;
			}

			for (auto&& [index, clipJson] : clipsArray)
			{
				AnimationClip clip;
				clip.loop = true;
				if (clipJson[U"loop"].getType() == JSONValueType::Bool)
				{
					clip.loop = clipJson[U"loop"].get<bool>();
				}
				const String clipName = clipJson[U"name"].getString();

				const auto& framesArray = clipJson[U"frames"];
				if (framesArray.getType() != JSONValueType::Array)
				{
					Console << U"Clip " << clipName << U" missing frames array";
					continue;
				}

				for (auto&& [fIndex, frameJson] : framesArray)
				{
					FrameData frame;
					const String path = frameJson[U"path"].getString();
					double duration = 0.1;
					if (frameJson[U"duration"].getType() == JSONValueType::Number)
					{
						duration = frameJson[U"duration"].get<double>();
					}
					frame.duration = duration;
					SizeF size = { 64, 113 };
					if (frameJson[U"width"].getType() == JSONValueType::Number && frameJson[U"height"].getType() == JSONValueType::Number)
					{
						size.x = frameJson[U"width"].get<double>();
						size.y = frameJson[U"height"].get<double>();
					}
					frame.size = size;
					// 画像をロード
					frame.texture = Texture(path);
					if (!frame.texture)
					{
						Console << U"Failed to load texture: " << path;
					}

					clip.frames.push_back(frame);
				}

				animator.AddClip(clipName, clip);
			}

			return true;
		}
	};
}

