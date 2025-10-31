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
		// クリップの名前と実際のデータ
		std::unordered_map<String, AnimationClip> clips;

		// 現在のBoolパラメータ値
		std::unordered_map<String, bool> boolParams;

		// 条件リスト（優先度順に評価される）
		std::vector<BoolCondition> conditionList;

		// 現在再生中のクリップ情報
		String currentClip;   // 再生中のクリップ名
		int currentFrame = 0; // 現在のフレームインデックス
		double frameTimer = 0.0; // 現在フレームの経過時間

		bool m_facingLeft = false;

		// 現在のBoolパラメータに応じてクリップを切り替える
		void UpdateCurrentClip() {
			std::optional<BoolCondition> selected;
			int maxPriority = std::numeric_limits<int>::min();

			for (auto& cond : conditionList) {
				bool match = true;
				for (auto& [param, value] : cond.conditions) {
					if (boolParams[param] != value) {
						match = false;
						break;
					}
				}
				if (match && cond.priority > maxPriority) {
					selected = cond;
					maxPriority = cond.priority;
				}
			}

			// 🔹 条件が一致しない場合はデフォルトクリップを使用
			String targetClip = selected ? selected->clipName : defaultClip;

			if (currentClip != targetClip) {
				currentClip = targetClip;
				currentFrame = 0;
				frameTimer = 0.0;
			}
		}

	public:
		void SetDefaultClip(const String& name) {
			defaultClip = name;
		}
		// クリップを登録
		void AddClip(const String& name, const AnimationClip& clip) {
			clips[name] = clip;
		}

		// Boolパラメータを更新
		void SetBool(const String& name, bool value) {
			boolParams[name] = value;
			UpdateCurrentClip(); // 変更があればクリップを切り替え
		}

		void SetFacingLeft(bool b) {
			m_facingLeft = b;
		}

		void SetBoolExclusive(const String& name) {
			// すべてのBoolをfalseにリセット
			for (auto& [key, val] : boolParams) {
				val = false;
			}

			// 指定したBoolだけtrueに
			boolParams[name] = true;

			// クリップ更新
			UpdateCurrentClip();
		}

		// 条件を登録
		void AddCondition(const BoolCondition& cond) {
			conditionList.push_back(cond);
		}

		// フレームタイマーの更新とフレーム切り替え
		void Update(double deltaTime) {
			if (clips.find(currentClip) == clips.end()) return;

			frameTimer += deltaTime;
			auto& frame = clips[currentClip].frames[currentFrame];

			// フレーム時間が経過したら次のフレームへ
			if (frameTimer >= frame.duration) {
				frameTimer -= frame.duration;
				currentFrame++;

				// 最後のフレームまで到達した場合の処理
				if (currentFrame >= (int)clips[currentClip].frames.size()) {
					currentFrame = clips[currentClip].loop ? 0 : (int)clips[currentClip].frames.size() - 1;
				}
			}
		}

		// 指定座標に描画
		void Draw(const Vec2& pos) const {
			if (clips.find(currentClip) == clips.end()) return;
			const auto& frame = clips.at(currentClip).frames[currentFrame];
			if (!frame.texture) return;

			const auto scaled = frame.texture.scaled(frame.scale);

			if (m_facingLeft)
				scaled.mirrored().drawAt(pos);
			else
				scaled.drawAt(pos);
		}

		void debug() {
			Print << U"Draw呼ばれた currentClip=" << currentClip
				<< U" frameIndex=" << currentFrame
				<< U" clip存在=" << (clips.contains(currentClip) ? U"Yes" : U"No")
				<< U" textureロード=" << (clips.contains(currentClip) ? (bool)clips.at(currentClip).frames[currentFrame].texture : false);

		}
	};
}
