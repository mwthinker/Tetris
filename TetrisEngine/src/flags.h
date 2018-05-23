#ifndef FLAGS_H
#define FLAGS_H

#include <ai.h>
#include <tetrisboard.h>
#include <calc/calculatorexception.h>

#include <chrono>
#include <queue>

class Flags {
public:
	Flags();

	Flags(const int argc, const char* const argv[]);

	void printHelpFunction() const;

	std::string programName_;
	bool printHelp_;
	std::chrono::milliseconds delay_;
	int maxNbrBlocks_;
	Ai ai_;
	bool play_;
	bool useRandomFile_;
	int width_;
	int height_;
	int verbose_;
	int depth_;
	std::string randomFilePath_;
	std::queue<std::string> outputOrder_;
};

#endif	// FLAGS_H
