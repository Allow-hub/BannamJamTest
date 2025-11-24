#include "Animator.h"

namespace Jam::Presentation
{
	void Animator::updateCurrentClip() {
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

		String targetClip = selected ? selected->clipName : defaultClip;

		if (currentClip != targetClip) {
			currentClip = targetClip;
			currentFrame = 0;
			frameTimer = 0.0;
		}
	}

	void Animator::setDefaultClip(const String& name) {
		defaultClip = name;
	}

	void Animator::addClip(const String& name, const AnimationClip& clip) {
		clips[name] = clip;
	}

	void Animator::setBool(const String& name, bool value) {
		boolParams[name] = value;
		updateCurrentClip();
	}

	void Animator::setFacingLeft(bool b) {
		m_facingLeft = b;
	}

	void Animator::setBoolExclusive(const String& name) {
		for (auto& [key, val] : boolParams) val = false;
		boolParams[name] = true;
		updateCurrentClip();
	}

	void Animator::addCondition(const BoolCondition& cond) {
		conditionList.push_back(cond);
	}

	void Animator::update(double deltaTime) {
		if (clips.find(currentClip) == clips.end()) return;

		frameTimer += deltaTime;
		auto& frame = clips[currentClip].frames[currentFrame];

		if (frameTimer >= frame.duration) {
			frameTimer -= frame.duration;
			currentFrame++;

			if (currentFrame >= (int)clips[currentClip].frames.size()) {
				currentFrame = clips[currentClip].loop ? 0 : (int)clips[currentClip].frames.size() - 1;
			}
		}
	}

	void Animator::draw(const Vec2& pos) const {
		if (clips.find(currentClip) == clips.end()) return;
		const auto& frame = clips.at(currentClip).frames[currentFrame];
		if (!frame.texture) return;

		const auto scaled = frame.texture.scaled(frame.scale);

		if (m_facingLeft)
			scaled.mirrored().drawAt(pos);
		else
			scaled.drawAt(pos);
	}
}
