#ifndef CALC_CALCULATOR_H
#define CALC_CALCULATOR_H

#include "symbol.h"
#include "cache.h"

#include <string>
#include <list>
#include <vector>
#include <functional>
#include <map>

namespace calc {

	class Calculator {
	public:
		friend class Cache;
		Calculator();

		Cache preCalculate(std::string infixNotation);

		float excecute(Cache cache);
		float excecute(std::string infixNotation);

		void addOperator(char token, char predence, bool leftAssociative, char parameters, const std::function<float(float, float)>& function);

		void addFunction(std::string name, char parameters, std::function<float(float, float)>& function);

		void addVariable(std::string name, float value);

		void updateVariable(std::string name, float value);

	private:
		// Returns a list of all symbols.
		std::list<Symbol> getSymbols(std::string infixNotation);

		std::vector<Symbol> shuntingYardAlgorithm(const std::list<Symbol>& infix);

		void initDefaultOperators();

		class ExcecuteFunction {
		public:
			ExcecuteFunction(char parameters, const std::function<float(float, float)>& function);

			float excecute();

			char parameters_;

			float param_[2];
		private:
			std::function<float(float, float)> function_;
		};

		std::map<std::string, Symbol> symbols_;
		std::vector<ExcecuteFunction> functions_;
		std::vector<float> variableValues_;
	};

} // Namespace calc;

#endif	// CALC_CALCULATOR_H
