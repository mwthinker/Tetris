#ifndef AI_H
#define AI_H

#include <string>
#include <vector>

#include "calc/calculator.h"
#include "rawtetrisboard.h"

class Ai {
public:
	inline Ai() : name_("DefaultAi") {
		valueFunction_ = "~5*rowRoughness - 10*columnRoughness + 10*meanHeight - 10*blockMeanHeight";
		initCalculator();
	}

	inline Ai(std::string name, std::string valueFunction) : name_(name), valueFunction_(valueFunction) {
		initCalculator();
	}

	inline std::string getName() const {
		return name_;
	}

	inline std::string getValueFunction() const {
		return valueFunction_;
	}

	inline calc::Calculator& getCalculator() {
		return calculator_;
	}

	inline const calc::Cache& getCache() {
		return cache_;
	}

	struct State {
		State() : down_(0), left_(0), rotations_(0) {
		}

		State(int down, int left, int rotations) : down_(down), left_(left), rotations_(rotations) {
		}

		int down_;
		int left_;
		int rotations_;
	};

	Ai::State calculateBestState(RawTetrisBoard board, int depth);
	
private:
	void initCalculator();
	
	Ai::State calculateBestState(RawTetrisBoard board, int depth, int removeRows);

	std::string name_;
	std::string valueFunction_;

	calc::Calculator calculator_;
	calc::Cache cache_;
};

#endif // AI_H
