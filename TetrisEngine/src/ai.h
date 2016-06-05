#ifndef AI_H
#define AI_H

#include <string>

class Ai {
public:
	inline Ai() : name_("DefaultAi") {
		valueFunction_ = "~5*rowRoughness - 10*columnRoughness + 10*meanHeight - 10*blockMeanHeight";
	}

	inline Ai(std::string name, std::string valueFunction) : name_(name), valueFunction_(valueFunction) {
	}

	inline std::string getName() const {
		return name_;
	}

	inline std::string getValueFunction() const {
		return valueFunction_;
	}

private:
	std::string name_;
	std::string valueFunction_;
};

#endif // AI_H
