#ifndef FLAGSEXCEPTION_H
#define FLAGSEXCEPTION_H

#include <string>

class FlagsException {
public:
	FlagsException() {
	}

	FlagsException(std::string error) : error_(error) {
	}

	std::string what() const {
		return error_;
	}

private:
	std::string error_;
};

#endif	// FLAGSEXCEPTION_H
