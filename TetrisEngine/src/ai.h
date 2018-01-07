#ifndef AI_H
#define AI_H

#include "rawtetrisboard.h"

#include <calc/calculator.h>

#include <string>
#include <vector>

class Ai {
public:
	Ai();

	Ai(std::string name, std::string valueFunction);

	std::string getName() const {
		return name_;
	}

	std::string getValueFunction() const {
		return valueFunction_;
	}

	const calc::Calculator& getCalculator() const {
		return calculator_;
	}

	const calc::Cache& getCache() {
		return cache_;
	}
	
	struct State {
		State();

		State(int left, int rotations);

		int left_;
		int rotationLeft_;
		float value_;
	};

	State calculateBestState(RawTetrisBoard board, int depth);
	
private:
	void initCalculator();
	
	State calculateBestStateRecursive(RawTetrisBoard board, int depth);

	std::string name_;
	std::string valueFunction_;

	calc::Calculator calculator_;
	calc::Cache cache_;
};

#endif // AI_H
