#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class Random {
public:
	Random() = default;
	Random(std::mt19937::result_type seed) : engine_(seed) {}

	int generateInt(int min, int max) const {
		return std::uniform_int_distribution<int>{min, max}(engine_);
	}
	
	float generateFloat(float min, float max) const {
		return std::uniform_real_distribution<float>{min, max}(engine_);
	}

	double generateDouble(double min, double max) const {
		return std::uniform_real_distribution<double>{min, max}(engine_);
	}

private:
	// To be able to use random in const functions.
	// The outer interface respect const.
	mutable std::mt19937 engine_{std::random_device{}()};
};

#endif // RANDOM_H
