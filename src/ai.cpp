#include "ai.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

// Return true if successful.
bool loadAi(Ai& ai, std::string dir, std::string str) {
	std::ifstream file(dir+str);
	if (file.is_open()) {
		std::vector<float> values;
		while (file.good()) {
			std::string line;
			std::getline(file, line);
			std::stringstream strStream(line);
			std::string word;
			float value;
			strStream >> word >> value; // E.g. word = "rowRoughness". calue = 10.1 .
			values.push_back(value);
		}
		file.close();
		if (values.size() == 7) {
			ai.rowRoughness_ = values[0];
			ai.columnRoughness_ = values[1];
			ai.edges_ = values[2];
			ai.meanHeight_ = values[3];
			ai.blockMeanHeight_ = values[4];
			ai.nbrOfRows_ = values[5];
			ai.nbrOfColumns_ = values[6];
			ai.name_ = str;
		}

		return true;
	}
	return false;
}

void saveAi(const Ai& ai, std::string dir) {
	std::stringstream stream;
	stream << "dir" << ai.getName();
	std::ofstream file(stream.str());
	if (file.is_open() && file.good()) {
		file << ai.rowRoughness_ << "\n";
		file << ai.columnRoughness_ << "\n";
		file << ai.edges_ << "\n";
		file << ai.meanHeight_ << "\n";
		file << ai.blockMeanHeight_ << "\n";
		file << ai.nbrOfRows_ << "\n";
		file << ai.nbrOfColumns_ << "\n";;
	}
}
