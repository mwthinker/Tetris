#include "symbol.h"

namespace calc {

	Operator Operator::create(char token, char predence, bool leftAssociative, char index) {
		Operator o;
		o.type_ = Type::OPERATOR;
		o.token_ = token;
		o.predence_ = predence;
		o.leftAssociative_ = leftAssociative;
		o.index_ = index;
		return o;
	}

	Paranthes Paranthes::create(bool left) {
		Paranthes p;
		p.type_ = Type::PARANTHES;
		p.left_ = left;
		return p;
	}

	Float Float::create(float value) {
		Float f;
		f.type_ = Type::FLOAT;
		f.value_ = value;
		return f;
	}

	Function Function::create(char index) {
		Function f;
		f.type_ = Type::FUNCTION;
		f.index_ = index;
		return f;
	}

	Comma Comma::create() {
		Comma c;
		c.type_ = Type::COMMA;
		return c;
	}

	Variable Variable::create(char index) {
		Variable v;
		v.type_ = Type::VARIABLE;		
		v.index_ = index;
		return v;
	}

} // Namespace calc;
