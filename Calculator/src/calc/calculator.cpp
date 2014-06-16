#include "calculator.h"

#include <sstream>
#include <stack>
#include <cmath>
#include <cassert>

namespace calc {

	Calculator::Operator Calculator::Operator::create(char token, char predence, bool leftAssociative, char index) {
		Operator o;
		o.type_ = Type::OPERATOR;
		o.token_ = token;
		o.predence_ = predence;
		o.leftAssociative_ = leftAssociative;
		o.index_ = index;
		return o;
	}

	Calculator::Paranthes Calculator::Paranthes::create(bool left) {
		Paranthes p;
		p.type_ = Type::PARANTHES;
		p.left_ = left;
		return p;
	}

	Calculator::Float Calculator::Float::create(float value) {
		Float f;
		f.type_ = Type::FLOAT;
		f.value_ = value;
		return f;
	}

	Calculator::Function Calculator::Function::create(char index) {
		Function f;
		f.type_ = Type::FUNCTION;
		f.index_ = index;
		return f;
	}

	Calculator::Comma Calculator::Comma::create() {
		Comma c;
		c.type_ = Type::COMMA;
		return c;
	}

	Calculator::Calculator() {
		initDefaultOperators();
	}

	void Calculator::initDefaultOperators() {
		addOperator('+', 2, true, 2, [](float a, float b) {
			return a + b;
		});

		addOperator('-', 2, true, 2, [](float a, float b) {
			return a - b;
		});

		addOperator('/', 3, true, 2, [](float a, float b) {
			return a / b;
		});

		addOperator('*', 3, true, 2, [](float a, float b) {
			return a * b;
		});

		addOperator('^', 4, false, 2, [](float a, float b) {
			return std::pow(a, b);
		});

		Comma c = Comma::create();
		Symbol symbol;
		symbol.comma_ = c;
		symbols_[","] = symbol;
		Paranthes p = Paranthes::create(true);
		symbol.paranthes_ = p;
		symbols_["("] = symbol;
		p = Paranthes::create(false);
		symbol.paranthes_ = p;
		symbols_[")"] = symbol;
	}

	float Calculator::excecute(std::string infixNotation) {
		std::list<Symbol> infix = getSymbols(infixNotation);
		std::vector<Symbol> prefix = shuntingYardAlgorithm(infix);
		int size = prefix.size();
		for (int index = 0; index < size; ++index) {
			Symbol& symbol = prefix[index];
			switch (symbol.type_) {
				case Type::FUNCTION:
					break;
				case Type::OPERATOR:
				{
					ExcecuteFunction& f = functions_[symbol.operator_.index_];
					int nbr = f.parameters_;

					// Symbols not already used?
					for (int j = index - 1; j >= 0 && nbr > 0; --j) {
						Type type = prefix[index].type_;
						if (prefix[j].type_ == Type::FLOAT) {
							// Set the parameter value.
							float b = prefix[j].float_.value_;
							f.param_[--nbr] = prefix[j].float_.value_;
							prefix[j].type_ = Type::NOTHING;
						}
					}
					if (nbr > 0) {
						// Error.
						return 0;
					}
					float value = f.excecute();
					symbol.float_ = Float::create(value);
				}
					break;
			}
		}

		if (size > 0) {
			return prefix[size - 1].float_.value_;
		}
		else return 0;
	}

	void Calculator::add(std::string variable, float value) {
		// Function name not used?
		if (symbols_.end() == symbols_.find(variable)) {
			Float f = Float::create(value);
			Symbol symbol;
			symbol.float_ = f;
			symbols_[variable] = symbol;
		}
	}

	// Returns a list of all symbols.
	std::list<Calculator::Symbol> Calculator::getSymbols(std::string infixNotation) {
		std::string text;
		for (char key : infixNotation) {
			std::string word;
			word += key;
			if (symbols_.end() != symbols_.find(word)) {
				text = text + " " + key + " ";
			} else {
				text += key;
			}
		}

		std::list<Symbol> infix;
		std::stringstream stream(text);
		std::string word;
		while (stream >> word) {
			if (symbols_.end() != symbols_.find(word)) {
				// Is a available symbol?
				infix.push_back(symbols_[word]);
			} else {
				float value;
				std::stringstream floatStream(word);
				if (floatStream >> value) { // Is a number?
					Symbol symbol;
					symbol.float_ = Float::create(value);
					infix.push_back(symbol);
				} else {
					assert(0);
					break;
				}
			}
		}
		return infix;
	}


	void Calculator::addOperator(char token, char predence, bool leftAssociative, char parameters, const std::function<float(float, float)>& function) {
		std::stringstream stream;
		stream << token;
		// Function name not used?
		if (symbols_.end() == symbols_.find(stream.str())) {
			Operator op = Operator::create(token, predence, leftAssociative, functions_.size());
			Symbol symbol;
			symbol.operator_ = op;
			symbols_[stream.str()] = symbol;
			functions_.push_back(ExcecuteFunction(parameters, function));
		}
	}

	void Calculator::addFunction(std::string name, char parameters, std::function<float(float, float)>& function) {
		// Function name not used?
		if (symbols_.end() == symbols_.find(name)) {
			Function f = Function::create(functions_.size());
			Symbol symbol;
			symbol.function_ = f;
			symbols_[name] = symbol;
			functions_.push_back(ExcecuteFunction(parameters, function));
		}
	}

	std::vector<Calculator::Symbol> Calculator::shuntingYardAlgorithm(const std::list<Symbol>& infix) {
		std::stack<Calculator::Symbol> operatorStack;
		std::vector<Symbol> output;
		for (const Symbol& symbol : infix) {
			switch (symbol.type_) {
				case Type::FLOAT:
					output.push_back(symbol);
					break;
				case Type::FUNCTION:
					operatorStack.push(symbol);
					break;
				case Type::COMMA:
					while (operatorStack.size() > 0) {
						Symbol top = operatorStack.top();
						// Is a left paranthes?
						if (top.type_ == Type::PARANTHES && top.paranthes_.left_) {
							break;
						} else { // Not a left paranthes.
							operatorStack.pop();
							output.push_back(top);
						}
					}
					break;
				case Type::OPERATOR:
					while (operatorStack.size() > 0 &&
						(((symbol.operator_.leftAssociative_ && symbol.operator_.predence_ == operatorStack.top().operator_.predence_)) ||
						(symbol.operator_.predence_ < operatorStack.top().operator_.predence_))) {
						output.push_back(operatorStack.top());
						operatorStack.pop();
					}
					operatorStack.push(symbol);
					break;
				case Type::PARANTHES:
					// Is left paranthes?
					if (symbol.paranthes_.left_) {
						operatorStack.push(symbol);
					} else {
						// Is right paranthes.

						if (operatorStack.size() < 1) {
							assert(0);
						}
						bool foundLeftParanthes = false;

						while (operatorStack.size() > 0) {
							Symbol top = operatorStack.top();
							operatorStack.pop();

							// Is a left paranthes?
							if (top.type_ == Type::PARANTHES && top.paranthes_.left_) {
								foundLeftParanthes = true;
								break;
							} else {
								// 'top' is not a left paranthes.
								output.push_back(top);
							}
						}

						if (operatorStack.size() > 0 && operatorStack.top().type_ == Type::FUNCTION) {
							output.push_back(operatorStack.top());
							operatorStack.pop();
						}

						if (!foundLeftParanthes) {
							// Error, mismatch of parantheses.
							assert(0);
							break;
						}
					}
					break;
			}
		}

		if (!operatorStack.empty()) {
			while (operatorStack.size() > 0) {
				Symbol top = operatorStack.top();
				if (top.type_ == Type::PARANTHES) {
					// Error, mismatch of parantheses.
					assert(0);
					break;
				}
				operatorStack.pop();
				output.push_back(top);
			}
		}
		return output;
	}

	Calculator::ExcecuteFunction::ExcecuteFunction(char parameters, const std::function<float(float, float)>& function) : function_(function), parameters_(parameters) {
	}

	float Calculator::ExcecuteFunction::excecute() {
		return function_(param_[0], param_[1]);
	}

} // Namespace calc;
