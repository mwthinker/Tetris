#include <ai.h>
#include <tetrisboard.h>
#include <calc/calculatorexception.h>

#include <fstream>
#include <sstream>
#include <thread>
#include <limits>
#include <queue>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace std::chrono_literals;

void printBoard(const TetrisBoard& board) {
	std::cout << "Current turn: " << board.getTurns() << std::endl;
	const std::vector<BlockType>& squares = board.getBoardVector();
	int rows = board.getRows();
	int columns = board.getColumns();
	Block block = board.getBlock();
	std::cout << "\n|";
	for (int row = rows - 3; row > -1; --row) {
		for (int column = 0; column < columns; ++column) {
			if (column == 0 && row < rows - 3) {
				// Add new row.
				std::cout << "  " << row + 2 << "\n|";
			}

			bool newSquare = false;
			for (Square sq : block) {
				if (sq.column_ == column && sq.row_ == row) {
					std::cout << "X|";
					newSquare = true;
					break;
				}
			}
			if (newSquare) {
				continue;
			}

			if (BlockType::EMPTY == squares[row * columns + column]) {
				std::cout << " |";
			} else {
				std::cout << "X|";
			}
		}
	}

	std::cout << "  1\n";
	for (int i = 0; i < columns; ++i) {
		std::cout << "--";
	}
	std::cout << "-\n\n";
}

BlockType badRandomBlockType() {
	static BlockType blockType = BlockType::Z;
	if (blockType == BlockType::Z) {
		blockType = BlockType::S;
	} else {
		blockType = BlockType::Z;
	}
	return blockType;
}

BlockType readBlockType(std::ifstream& infile) {
	int data = -1;
	if (infile.is_open()) {
		if (!(infile >> data)) {
			// Restart, read from the start again.
			infile.clear();
			infile.seekg(0, std::ios::beg);
			infile >> data;
		}
	}
	if (data >= 0 && data <= 6) {
		return (BlockType) data;
	}
	// In order for the ai to fail.
	return badRandomBlockType();
}

void printHelpFunction(std::string programName, const Ai& ai) {
	std::cout << "Usage: " << programName << "\n";
	std::cout << "\t" << "Simulate a tetris game, using a ai value-funtion.\n";
	std::cout << "\t" << "Default ai value function is \"" << ai.getValueFunction() << "\".\n";
	std::cout << "\t" << programName << " -d <DELAY>\n";
	std::cout << "\t" << programName << " -a <VALUE_FUNCTION>\n";
	std::cout << "\t" << programName << " -m <MAX_TURNS>\n";
	std::cout << "\t" << programName << " -f <FILE>\n";
	std::cout << "\t" << programName << " -s <WIDTH> <HEIGHT>\n\n";

	std::cout << "\t" << "Variables available in the value function:\n";
	for (std::string var : ai.getCalculator().getVariables()) {
		std::cout << "\t" << var << "\n";
	}

	std::cout << "\n";
	std::cout << "\tIf using the -f flag, the file provided must be a text file and be filled with\n";
	std::cout << "\tnumber, space, number etc. Other characters will be ignored.\n";
	std::cout << "\tBlocktypes: \n";
	std::cout << "\tI = 0\n";
	std::cout << "\tJ = 1\n";
	std::cout << "\tL = 2\n";
	std::cout << "\tO = 3\n";
	std::cout << "\tS = 4\n";
	std::cout << "\tT = 5\n";
	std::cout << "\tZ = 6\n";
	std::cout << "\tExample of data in file is \"0 2 0 7 1 3\".\n";
	std::cout << "\tWhen the simulation has used the whole file, it start to read from the beginning again, and so on.\n\n";

	std::cout << "Options: " << "\n";
	std::cout << "\t-h --help                show this help\n";
	std::cout << "\t-d --delay               delay in milliseconds between each turn\n";
	std::cout << "\t-a --ai-function         define the value function to be use by the ai\n";
	std::cout << "\t-m --max-turns           define the max number of turns\n";
	std::cout << "\t-f --file-data           use random data from a file\n";
	std::cout << "\t-s --board-size          define the size of the board\n";
	std::cout << "\t-p --play                show board each turn\n\n";
	std::cout << "\tOutput order:\n";
	std::cout << "\t-T --time                print the time lapsed\n";
	std::cout << "\t-t --turns               print the number of turns\n";
	std::cout << "\t-c1 --cleared-row-1      print cleared one rows\n";
	std::cout << "\t-c2 --cleared-row-2      print cleared two rows\n";
	std::cout << "\t-c3 --cleared-row-3      print cleared three rows\n";
	std::cout << "\t-c4 --cleared-row-4      print cleared four rows\n\n";

	std::cout << "Example: " << "\n";
	std::cout << "\tShow the default result for the default ai, in the same order as the abowe output order.\n";
	std::cout << "\t" << programName << "\n\n";

	std::cout << "\tShow the tetrisboard each turn and delay 250 ms between each turn.\n";
	std::cout << "\t" << programName << "--play -d 250\n\n";

	std::cout << "\tList the time and number of four-rows cleared.\n";
	std::cout << "\t" << programName << "-T --cleared-row-4\n";
	std::exit(0);
}

std::istream& operator>>(std::istream& is, std::chrono::milliseconds& time) {
	int intTime;
	is >> intTime;
	time = std::chrono::milliseconds(intTime);
	return is;
}

int main(const int argc, const char* const argv[]) {
	std::string programName;
	if (argc > 0) {
		programName = argv[0];
	}

	std::chrono::milliseconds delay(0);
	int maxNbrBlocks = std::numeric_limits<int>::max();
	Ai ai;
	bool play = false;
	bool useRandomFile = false;

	int width = 10;
	int height = 24;

	std::queue<std::string> outputOrder;
	std::ifstream infile;

	for (int i = 0; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "-h" || arg == "--help") {
			printHelpFunction(programName, ai);
		} else if (arg == "-d" || arg == "--delay") {
			if (i + 1 < argc) {
				std::stringstream stream(argv[i + 1]);
				stream >> delay;
				++i;
			} else {
				std::cerr << "Missing argument after " << arg << " flag\n";
				std::exit(1);
			}
		} else if (arg == "-a" || arg == "--ai") {
			if (i + 1 < argc) {
				std::string valueFunction = argv[i + 1];
				try {
					ai = Ai("AI", valueFunction);
				} catch (calc::CalculatorException exception) {
					std::cerr << "Value function error: ";
					std::cerr << exception.what() << "\n";
					std::exit(1);
				}
				++i;
			} else {
				std::cerr << "Missing argument after " << arg << " flag\n";
				std::exit(1);
			}
		} else if (arg == "-m" || arg == "--max-nbr-blocks") {
			if (i + 1 < argc) {
				std::stringstream stream(argv[i + 1]);
				stream >> maxNbrBlocks;
				++i;
			} else {
				std::cerr << "Missing argument after " << arg << " flag\n";
				std::exit(1);
			}
		} else if (arg == "-f" || arg == "--file-data") {
			if (i + 1 < argc) {
				infile.open(argv[i + 1]);
				useRandomFile = true;
				++i;
			} else {
				std::cerr << "Missing argument after " << arg << " flag\n";
				std::exit(1);
			}
		} else if (arg == "-z" || arg == "--board-size") {
			if (i + 2 < argc) {
				std::stringstream stream;
				stream << argv[i + 1];
				stream << argv[i + 2];
				stream >> width;
				stream >> height;
				i += 2;
				if (width < 5 || width > 99) {
					std::cerr << "Argument with flag " << arg << ", width " << width << " must be within [5, 99]\n";
					std::exit(1);
				}
				if (height < 5 || height > 99) {
					std::cerr << "Argument with flag " << arg << ", height " << width << " must be within [5, 99]\n";
					std::exit(1);
				}
			} else {
				std::cerr << "Missing argument after " << arg << " flag\n";
				std::exit(1);
			}
		} else if (arg == "-p" || arg == "--play") {
			play = true;
		} else if (arg == "-T" || arg == "--time-output") {
			outputOrder.push("-T");
		} else if (arg == "-t" || arg == "--turn-output") {
			outputOrder.push("-t");
		} else if (arg == "-c1" || arg == "--cleared-row-1") {
			outputOrder.push("-c1");
		} else if (arg == "-c2" || arg == "--cleared-row-2") {
			outputOrder.push("-c2");
		} else if (arg == "-c3" || arg == "--cleared-row-3") {
			outputOrder.push("-c3");
		} else if (arg == "-c4" || arg == "--cleared-row-4") {
			outputOrder.push("-c4");
		}
	}

	BlockType start = randomBlockType();
	BlockType next = randomBlockType();

	if (useRandomFile) {
		start = readBlockType(infile);
		next = readBlockType(infile);
	}

	TetrisBoard tetrisBoard(width, height, start, next);
	int nbrOneLine = 0;
	int nbrTwoLine = 0;
	int nbrThreeLine = 0;
	int nbrFourLine = 0;

	tetrisBoard.addGameEventListener([&](GameEvent gameEvent, const TetrisBoard&) {
		if (GameEvent::BLOCK_COLLISION == gameEvent) {
			if (useRandomFile) {
				BlockType blockType = readBlockType(infile);
				tetrisBoard.updateNextBlock(blockType);
			} else {
				tetrisBoard.updateNextBlock(randomBlockType());
			}
		}
		switch (gameEvent) {
			case GameEvent::ONE_ROW_REMOVED:
				++nbrOneLine;
				break;
			case GameEvent::TWO_ROW_REMOVED:
				++nbrTwoLine;
				break;
			case GameEvent::THREE_ROW_REMOVED:
				++nbrThreeLine;
				break;
			case GameEvent::FOUR_ROW_REMOVED:
				++nbrFourLine;
				break;
		}
	});

	auto time = std::chrono::high_resolution_clock::now();
	while (!tetrisBoard.isGameOver() && tetrisBoard.getTurns() < maxNbrBlocks) {
		Ai::State state = ai.calculateBestState(tetrisBoard, 1);

		if (play) {
			std::cout << "AI: " << ai.getValueFunction() << "\n";
			std::cout << "Turns: " << tetrisBoard.getTurns() << "\n";
			printBoard(tetrisBoard);
		}

		// Rotate block.
		for (int i = 0; i < state.rotationLeft_; ++i) {
			tetrisBoard.update(Move::ROTATE_LEFT);
		}

		// Move side-ways.
		while (state.left_ != 0) {
			if (state.left_ < 0) {
				tetrisBoard.update(Move::RIGHT);
				++state.left_;
			} else if (state.left_ > 0) {
				tetrisBoard.update(Move::LEFT);
				--state.left_;
			}
		}

		tetrisBoard.update(Move::DOWN_GROUND);
		tetrisBoard.update(Move::DOWN_GRAVITY);
		if (delay > 1ms) {
			std::this_thread::sleep_for(delay);
		}
	}
	std::chrono::duration<double> delta = std::chrono::high_resolution_clock::now() - time;

	std::cout << std::setprecision(2) << std::fixed;
	if (play) {
		printBoard(tetrisBoard);
		std::cout << "AI: " << ai.getValueFunction() << "\n";
		std::cout << "Turns: " << tetrisBoard.getTurns() << "\n";
		std::cout << "Time in seconds: " << delta.count() << "\n";
	} else if (outputOrder.empty()) {
		outputOrder.push("-T");
		outputOrder.push("-t");
		outputOrder.push("-c1");
		outputOrder.push("-c2");
		outputOrder.push("-c3");
		outputOrder.push("-c4");
	}

	while (!outputOrder.empty()) {
		std::string flag = outputOrder.front();
		if (flag == "-T") {
			std::cout << delta.count() << "\t";
		} else if (flag == "-t") {
			std::cout << tetrisBoard.getTurns() << "\t";
		} else if (flag == "-c1") {
			std::cout << nbrOneLine << "\t";
		} else if (flag == "-c2") {
			std::cout << nbrTwoLine << "\t";
		} else if (flag == "-c3") {
			std::cout << nbrThreeLine << "\t";
		} else if (flag == "-c4") {
			std::cout << nbrFourLine << "\t";
		}
		outputOrder.pop();
	}

	std::cout << "\n";
	return 0;
}
