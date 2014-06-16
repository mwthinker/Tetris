#ifndef CALC_SYMBOL_H
#define CALC_SYMBOL_H

namespace calc {

	enum class Type : char {
		OPERATOR,
		FLOAT,
		FUNCTION,
		PARANTHES,
		COMMA,
		VARIABLE,
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

	class Variable {
	public:
		static Variable create(char index);

		Type type_;
		char index_;
	};

	union Symbol {
		Type type_;
		Operator operator_;
		Paranthes paranthes_;
		Float float_;
		Function function_;
		Comma comma_;
		Variable variable_;
	};


} // Namespace calc

#endif	// CALC_SYMBOL_H
