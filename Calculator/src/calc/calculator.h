#ifndef CALC_CALCULATOR_H
#define CALC_CALCULATOR_H

#include <string>
#include <list>
#include <vector>
#include <functional>
#include <map>

namespace calc {

	class Calculator {
	public:
		Calculator();		

		float excecute(std::string infixNotation);

		void addOperator(char token, char predence, bool leftAssociative, char parameters, const std::function<float(float, float)>& function);

		void addFunction(std::string name, char parameters, std::function<float(float, float)>& function);
		
		void add(std::string variable, float value);		

	private:
		enum class Type : char {
			OPERATOR,
			FLOAT,
			FUNCTION,
			PARANTHES,
			COMMA,
			NOTHING
		};

		class Operator {
		public:
			static Operator create(char token, char predence, bool leftAssociative, char index);

			Type type_;
			char token_;
			char predence_;
			bool leftAssociative_;
			char index_;
		};

		class Paranthes {
		public:
			static Paranthes create(bool left);

			Type type_;
			bool left_;
		};

		class Float {
		public:
			static Float create(float value);

			Type type_;
			float value_;
		};

		class Function {
		public:
			static Function create(char index);

			Type type_;
			char index_;
		};
		
		class Comma {
		public:
			static Comma create();

			Type type_;
		};

		union Symbol {
			Type type_;
			Operator operator_;
			Paranthes paranthes_;
			Float float_;
			Function function_;
			Comma comma_;
		};		

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
	};

} // Namespace calc;

#endif	// CALC_CALCULATOR_H
