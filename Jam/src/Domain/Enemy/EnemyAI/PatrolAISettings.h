#pragma once

namespace Jam::Domain::Enemy
{
	struct PatrolPoint {
		Vec2 pos;
	};

	struct PatrolAIExtra {
		Array<PatrolPoint> patrolPoints;
		bool loop = false;
		double waitTime = 0.0;
	};

	struct PatrolRoute {
		Array<Vec2> points;
		bool loop = false;
		double waitTime = 0.0;
		size_t currentIndex = 0;
		double waitTimer = 0.0;

		bool isValid() const { return not points.isEmpty(); }

		void advance() {
			if (points.isEmpty()) return;
			currentIndex++;
			if (currentIndex >= points.size()) {
				currentIndex = loop ? 0 : (points.size() - 1);
			}
		}

		void resetTimer() { waitTimer = 0; }
		void updateTimer(double dt) { waitTimer += dt; }
		bool isWaitOver() const { return waitTimer >= waitTime; }
	};
}
