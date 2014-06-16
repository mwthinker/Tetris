#include "calc/calculator.h"

#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <stack>
#include <vector>
#include <cmath>
#include <list>
#include <functional>

using namespace calc;

int main() {
	Calculator calculator;	
	std::string inFix = "3 + 4 * 2 / ( 1 -5) ^2^3";
	//std::string inFix = "3 + 4*2";
	std::cout << inFix << "\n";

	std::cout << calculator.excecute(inFix) << "\n";
	/*
	std::string formatted = calculator.formattedText(inFix);
	std::cout << formatted << "\n";
	std::string postFix = calculator.shuntingYardAlgorithm(formatted);
	std::cout << postFix << "\n";
	*/
	return 0;
}
