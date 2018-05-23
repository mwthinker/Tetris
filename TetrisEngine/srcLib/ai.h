#ifndef AI_H
#define AI_H

#include "rawtetrisboard.h"

#include <calc/calculator.h>

#include <string>
#include <vector>

struct RowRoughness {
	RowRoughness() : holes_(0), rowSum_(0) {
	}

	int holes_;
	int rowSum_;
};

struct ColumnRoughness {
	ColumnRoughness() : holes_(0), bumpiness(0) {
	}

	int holes_;
	int bumpiness;
};

RowRoughness calculateRowRoughness(const RawTetrisBoard& board, int highestUsedRow);
ColumnRoughness calculateColumnHoles(const RawTetrisBoard& board, int highestUsedRow);
int calculateHighestUsedRow(const RawTetrisBoard& board);
float calculateBlockMeanHeight(const Block& block);
int calculateBlockEdges(const RawTetrisBoard& board, const Block& block);

int calculateLandingHeight(const Block& block);
int calculateErodedPieces(const RawTetrisBoard& board);
int calculateRowTransitions(const RawTetrisBoard& board);
int calculateColumnTransitions(const RawTetrisBoard& board);
int calculateNumberOfHoles(const RawTetrisBoard& board);
int calculateCumulativeWells(const RawTetrisBoard& board);
int calculateHoleDepth(const RawTetrisBoard& board);

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

	State calculateBestState(const RawTetrisBoard& board, int depth);
	float moveBlockToGroundCalculateValue(const State& state, RawTetrisBoard& board);
	
private:
	void initCalculator();
	
	State calculateBestStateRecursive(const RawTetrisBoard& board, int depth);

	std::string name_;
	std::string valueFunction_;

	calc::Calculator calculator_;
	calc::Cache cache_;
};

void moveBlockToBeforeImpact(const Ai::State& state, RawTetrisBoard& board);

#endif // AI_H
