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
int calculateRowHoles(const RawTetrisBoard& board);

struct AiParameters {
	inline AiParameters() : landingHeight_(false), erodedPieces_(false),
		rowHoles_(false), columnHoles_(false),
		holes_(false), cumulativeWells_(false),
		holeDepth_(false) {
	}

	bool landingHeight_;
	bool erodedPieces_;
	bool rowHoles_;
	bool columnHoles_;
	bool holes_;
	bool cumulativeWells_;
	bool holeDepth_;
};

class Ai {
public:
	Ai();

	Ai(std::string name, std::string valueFunction, bool allowException = false);

	std::string getName() const {
		return name_;
	}

	std::string getValueFunction() const {
		return valueFunction_;
	}

	const calc::Calculator& getCalculator() const {
		return calculator_;
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

	static constexpr const char* getDefaultValueFunction() {
		return "-0.2*cumulativeWells - 1*holeDepth - 1*holes - 1*landingHeight";
	}
	
private:
	void initCalculator(bool allowException);
	void initAiParameters(const calc::Calculator& calculator, const calc::Cache& cache);
	
	State calculateBestStateRecursive(const RawTetrisBoard& board, int depth);

	std::string name_;
	std::string valueFunction_;

	calc::Calculator calculator_;
	calc::Cache cache_;
	AiParameters parameters_;
};

void moveBlockToBeforeImpact(const Ai::State& state, RawTetrisBoard& board);

#endif // AI_H
