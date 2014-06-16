#ifndef CACHE_CALCULATOR_H
#define CACHE_CALCULATOR_H

#include "symbol.h"

#include <vector>

namespace calc {	

	class Cache {
	public:
		friend class Calculator;
		
		Cache();
		
	private:
		Cache(const std::vector<Symbol>& symbols);

		std::vector<Symbol> symbols_;
	};

} // Namespace calc;

#endif	// CACHE_CALCULATOR_H
