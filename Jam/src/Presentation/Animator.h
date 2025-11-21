#pragma once
#include <Siv3D.hpp>
#include <unordered_map>
#include <vector>
#include <optional>

namespace Jam::Presentation
{
	// --- 1フレームの情報 ---
	struct FrameData {
		Texture texture;   // このフレームで表示する画像
		double duration;   // 表示時間（秒）
		double scale = 1.0;
	};

	// --- アニメーションクリップ ---
	struct AnimationClip {
		std::vector<FrameData> frames; // クリップに含まれるフレーム
		bool loop = true;              // ループするかどうか
	};

	// --- Boolパラメータに基づく再生条件 ---
	struct BoolCondition {
		std::unordered_map<String, bool> conditions; // Bool名 -> 必須値（条件）
		String clipName;                             // 条件に一致したときの再生クリップ名
		int priority = 0;                            // 条件の優先度。大きいほど優先
	};

	// --- アニメーター ---
	class Animator {
	private:
		String defaultClip = U"Idle";
		std::unordered_map<String, AnimationClip> clips;
		std::unordered_map<String, bool> boolParams;
		std::vector<BoolCondition> conditionList;

		String currentClip;
		int currentFrame = 0;
		double frameTimer = 0.0;

		bool m_facingLeft = false;

		void updateCurrentClip();

	public:
		void setDefaultClip(const String& name);

		//クリップを追加
		void addClip(const String& name, const AnimationClip& clip);
		void setBool(const String& name, bool value);

		//左を向いているかどうかでTextureを反転
		void setFacingLeft(bool b);

		//指定したブール以外をfalseに
		void setBoolExclusive(const String& name);

		//Clipの遷移を追加
		void addCondition(const BoolCondition& cond);
		void update(double deltaTime);
		void draw(const Vec2& pos) const;
	};
}
