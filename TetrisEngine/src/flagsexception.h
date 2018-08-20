#ifndef FLAGSEXCEPTION_H
#define FLAGSEXCEPTION_H

#include <string>
#include <stdexcept>

class FlagsException : public std::runtime_error {
public:
	explicit FlagsException() : std::runtime_error("") {
	}

	explicit FlagsException(const char* error) : std::runtime_error(error) {
	}

	explicit FlagsException(const std::string& error) : std::runtime_error(error) {
	}
};

#endif	// FLAGSEXCEPTION_H
