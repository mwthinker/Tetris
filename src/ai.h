#ifndef AI_H
#define AI_H

#include <string>

class Ai {
public:
	friend bool loadAi(Ai&, std::string, std::string);
	friend void saveAi(const Ai&, std::string);

	inline Ai() : rowRoughness_(-5), columnRoughness_(-10), edges_(0), meanHeight_(10),
		blockMeanHeight_(-10), nbrOfRows_(0), nbrOfColumns_(0), name_("DefaultAi") {
	}

	inline Ai(std::string name, double rowRoughness, double columnRoughness, double edges, double meanHeight,
		double blockMeanHeight, double nbrOfRows, double nbrOfColumns) : rowRoughness_(rowRoughness), columnRoughness_(columnRoughness), edges_(edges), meanHeight_(meanHeight),
		blockMeanHeight_(blockMeanHeight), nbrOfRows_(nbrOfRows), nbrOfColumns_(nbrOfColumns), name_(name) {
	}

	inline double calculateValue(double rowRoughness, double columnRoughness, double edges, double meanHeight,
		double blockMeanHeight, double nbrOfRows, double nbrOfColumns) const {
		return rowRoughness_*rowRoughness + columnRoughness_*columnRoughness + edges_*edges + meanHeight_*meanHeight +
			blockMeanHeight_*blockMeanHeight + nbrOfRows_*nbrOfRows + nbrOfColumns_*nbrOfColumns;
	}

	inline std::string getName() const {
		return name_;
	}

private:
	double rowRoughness_;
	double columnRoughness_;
	double edges_;
	double meanHeight_;
	double blockMeanHeight_;	
	double nbrOfRows_;
	double nbrOfColumns_;

	std::string name_;
};

// Return true if successful and stores the loaded ai in (ai).
bool loadAi(Ai& ai, std::string dir, std::string str);

// Saves tha ai in directory (dir) and with the same filename as the ai.
void saveAi(const Ai& ai, std::string dir);

#endif // AI_H
