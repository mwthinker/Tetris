#ifndef AI_H
#define AI_H

class Ai {
public:
	Ai() : rowRoughness_(-5), columnRoughness_(-10), edges_(0), meanHeight_(10),
		blockMeanHeight_(-10), nbrOfRows_(0), nbrOfColumns_(0) {
	}

	Ai(double rowRoughness, double columnRoughness, double edges, double meanHeight,
		double blockMeanHeight, double nbrOfRows, double nbrOfColumns) : rowRoughness_(rowRoughness), columnRoughness_(columnRoughness), edges_(edges), meanHeight_(meanHeight),
		blockMeanHeight_(blockMeanHeight), nbrOfRows_(nbrOfRows), nbrOfColumns_(nbrOfColumns) {
	}

	inline double calculateValue(double rowRoughness, double columnRoughness, double edges, double meanHeight,
		double blockMeanHeight, double nbrOfRows, double nbrOfColumns) const {
		return rowRoughness_*rowRoughness + columnRoughness_*columnRoughness + edges_*edges + meanHeight_*meanHeight +
			blockMeanHeight_*blockMeanHeight + nbrOfRows_*nbrOfRows + nbrOfColumns_*nbrOfColumns;
	}

private:
	double rowRoughness_;
	double columnRoughness_;
	double edges_;
	double meanHeight_;
	double blockMeanHeight_;	
	double nbrOfRows_;
	double nbrOfColumns_;
};

#endif // AI_H