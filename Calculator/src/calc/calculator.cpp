#include "calculator.h"

#include <sstream>
#include <stack>
#include <cmath>
#include <cassert>
#include <algorithm>

namespace calc {

	Calculator::Calculator() : error_(false) {
		initDefaultOperators();
	}

	void Calculator::initDefaultOperators() {
		// Unary minus sign operator.
		addOperator('~', 5, false, 1, [](float a, float b) {
			return -a;
		});

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

	Cache Calculator::preCalculate(std::string infixNotation) {
		error_ = false; // Reset error;
		std::list<Symbol> infix = getSymbols(infixNotation);
		if (error_) {
			return Cache();
		}
		Cache cache(shuntingYardAlgorithm(infix));
		if (error_) {
			return Cache();
		}
		return cache;
	}

	float Calculator::excecute(Cache cache) {
		error_ = false; // Reset error;
		std::vector<Symbol>& prefix = cache.symbols_;
		int size = prefix.size();
		for (int index = 0; index < size; ++index) {
			Symbol& symbol = prefix[index];
			ExcecuteFunction* f = nullptr;
			switch (symbol.type_) {
				case Type::FUNCTION:
					f = &functions_[symbol.function_.index_];
					// Fall through!
				case Type::OPERATOR:
				{
					if (f == nullptr) {
						// Is an operator, not a function!
						f = &functions_[symbol.operator_.index_];
					}
					int nbr = f->parameters_;

					// Symbols not already used?
					for (int j = index - 1; j >= 0 && nbr > 0; --j) {
						if (prefix[j].type_ == Type::FLOAT) {
							// Set the parameter value.
							f->param_[--nbr] = prefix[j].float_.value_;
							prefix[j].type_ = Type::NOTHING;
						} else if (prefix[j].type_ == Type::VARIABLE) {
							// Set the parameter value.
							f->param_[--nbr] = variableValues_[prefix[j].variable_.index_];
							prefix[j].type_ = Type::NOTHING;
						}
					}
					if (nbr > 0) {
						error_ = true;
						auto it = std::find_if(symbols_.begin(), symbols_.end(), [&](const std::pair<std::string, Symbol>& pair) {
							if (symbol.type_ == Type::FUNCTION) {
								return pair.second.function_.index_ == symbol.function_.index_;
							} else { // Is a operator!
								return pair.second.operator_.token_ == symbol.operator_.index_;
							}
						});
						if (it != symbols_.end()) {
							errorMessage_ = "\nFunction/operator ";
							errorMessage_ += it->first;
							errorMessage_ += " missing enough parameters.";
						} else {
							errorMessage_ = "\nUnrecognized symbol in cachce.";
						}
						return 0;
					}
					float value = f->excecute();
					symbol.float_ = Float::create(value);
					break;
				}
				default:
					// Continue the iteration!
					break;
			}
		}

		if (size > 0) {
			return prefix[size - 1].float_.value_;
		} else return 0;
	}

	float Calculator::excecute(std::string infixNotation) {
		Cache cache = preCalculate(infixNotation);
		return excecute(cache);
	}

	void Calculator::addVariable(std::string name, float value) {
		// Function name not used?
		if (symbols_.end() == symbols_.find(name)) {
			Variable v = Variable::create(variableValues_.size());
			variableValues_.push_back(value);
			Symbol symbol;
			symbol.variable_ = v;
			symbols_[name] = symbol;
		}
	}

	void Calculator::updateVariable(std::string name, float value) {
		variableValues_[symbols_[name].variable_.index_] = value;
	}

	// Returns a list of all symbols.
	std::list<Symbol> Calculator::getSymbols(std::string infixNotation) {
		std::string text;
		// Add space between all "symbols" 
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
					error_ = true;
					errorMessage_ = "\nUnrecognized symbol: ";
					errorMessage_ += word;
					return std::list<Symbol>();
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

	bool Calculator::hasError() const {
		return error_;
	}

	std::string Calculator::getErrorMessage() const {
		return errorMessage_;
	}

	std::vector<Symbol> Calculator::shuntingYardAlgorithm(const std::list<Symbol>& infix) {
		std::stack<Symbol> operatorStack;
		std::vector<Symbol> output;
		for (const Symbol& symbol : infix) {
			switch (symbol.type_) {
				case Type::VARIABLE:
					// Fall through!
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
					} else { // Is right paranthes.
						if (operatorStack.size() < 1) {
							error_ = true;
							errorMessage_ = "\nMissing right parameter '(' in expression.";
							return std::vector<Symbol>();
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
							error_ = true;
							errorMessage_ = "\nError, mismatch of parantheses in expression";
							return std::vector<Symbol>();
						}
					}
					break;
			}
		}

		if (!operatorStack.empty()) {
			while (operatorStack.size() > 0) {
				Symbol top = operatorStack.top();
				if (top.type_ == Type::PARANTHES) {
					error_ = true;
					errorMessage_ = "\nError, mismatch of parantheses in expression";
					return std::vector<Symbol>();
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
